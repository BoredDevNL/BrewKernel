#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "errno.h"
#include "../../filesys.h"
#include "../../file.h"

struct __bk_file {
	char mode; // 'r' or 'w'
	char* path;
	const char* r_data;
	size_t r_size;
	size_t pos;
	char* w_buf;
	size_t w_size;
	size_t w_cap;
};

int errno = 0;

static int ensure_w_cap(struct __bk_file* f, size_t need) {
	if (f->w_cap >= need) return 1;
	size_t ncap = f->w_cap ? f->w_cap * 2 : 512;
	while (ncap < need) ncap *= 2;
	char* nbuf = (char*)realloc(f->w_buf, ncap);
	if (!nbuf) return 0;
	f->w_buf = nbuf;
	f->w_cap = ncap;
	return 1;
}

FILE* fopen(const char* path, const char* mode) {
	struct __bk_file* f = (struct __bk_file*)malloc(sizeof(struct __bk_file));
	if (!f) return 0;
	memset(f, 0, sizeof(*f));
	f->path = strdup(path);
	if (!f->path) { free(f); return 0; }
	if (mode && mode[0] == 'r') {
		f->mode = 'r';
		size_t sz = 0;
		// Check if directory:
		File* found = fs_find_file(path);
		if (found && found->type == 'd') {
			errno = EISDIR;
			free(f->path); free(f);
			return 0;
		}
		f->r_data = fs_read_file_at_path(path, &sz);
		if (!f->r_data) {
			errno = ENOENT;
			free(f->path); free(f);
			return 0;
		}
		f->r_size = sz;
		f->pos = 0;
	} else if (mode && mode[0] == 'w') {
		f->mode = 'w';
		f->w_buf = 0;
		f->w_size = 0;
		f->w_cap = 0;
	} else {
		errno = EINVAL;
		free(f->path); free(f);
		return 0;
	}
	return f;
}

int fclose(FILE* f) {
	if (!f) return 0;
	if (f->mode == 'w') {
		if (f->w_size > 0) {
			if (!fs_write_file_at_path(f->path, f->w_buf, f->w_size)) {
				errno = EIO;
			}
		} else {
			// Create empty file
			(void)fs_create_file_at_path(f->path);
		}
	}
	free(f->w_buf);
	free(f->path);
	free(f);
	return 0;
}

size_t fread(void* ptr, size_t size, size_t nmemb, FILE* f) {
	if (!f || f->mode != 'r') return 0;
	size_t bytes = size * nmemb;
	if (f->pos >= f->r_size) return 0;
	size_t avail = f->r_size - f->pos;
	if (bytes > avail) bytes = avail;
	memcpy(ptr, f->r_data + f->pos, bytes);
	f->pos += bytes;
	return size ? (bytes / size) : 0;
}

size_t fwrite(const void* ptr, size_t size, size_t nmemb, FILE* f) {
	if (!f || f->mode != 'w') return 0;
	size_t bytes = size * nmemb;
	if (!ensure_w_cap(f, f->w_size + bytes)) return 0;
	memcpy(f->w_buf + f->w_size, ptr, bytes);
	f->w_size += bytes;
	return nmemb;
}

long ftell(FILE* f) {
	if (!f) return -1;
	return (long)f->pos;
}

int fseek(FILE* f, long offset, int whence) {
	if (!f) return -1;
	size_t base = 0;
	if (whence == SEEK_SET) base = 0;
	else if (whence == SEEK_CUR) base = f->pos;
	else if (whence == SEEK_END) base = (f->mode == 'r') ? f->r_size : f->w_size;
	else return -1;
	long np = (long)base + offset;
	if (np < 0) np = 0;
	size_t nps = (size_t)np;
	if (f->mode == 'r') {
		if (nps > f->r_size) nps = f->r_size;
	} else {
		if (nps > f->w_size) {
			// grow with zeros
			if (!ensure_w_cap(f, nps)) return -1;
			memset(f->w_buf + f->w_size, 0, nps - f->w_size);
			f->w_size = nps;
		}
	}
	f->pos = nps;
	return 0;
}

int fflush(FILE* f) {
	(void)f;
	return 0;
}

int remove(const char* path) {
	return fs_remove_file(path) ? 0 : -1;
}

int rename(const char* oldpath, const char* newpath) {
	size_t sz = 0;
	const char* data = fs_read_file_at_path(oldpath, &sz);
	if (!data) return -1;
	if (!fs_write_file_at_path(newpath, data, sz)) return -1;
	(void)fs_remove_file(oldpath);
	return 0;
}

// Simple sscanf supporting the few patterns used in m_misc.c (hex, oct, dec ints)
int sscanf(const char* str, const char* fmt, ...) {
	// We ignore whitespace in format and accept one integer
	(void)fmt;
	va_list ap;
	va_start(ap, fmt);
	int* out = va_arg(ap, int*);
	va_end(ap);
	if (!out) return 0;
	// Detect base: hex if starts with 0x/0X, oct if starts with 0, else dec
	int base = 10;
	const char* s = str;
	while (isspace((int)*s)) ++s;
	if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) base = 16;
	else if (s[0] == '0' && s[1] >= '0' && s[1] <= '7') base = 8;
	char* endp = 0;
	long v = strtol(s, &endp, base);
	if (endp == s) return 0;
	*out = (int)v;
	return 1;
}

// Implement mkdir via ramfs
int mkdir(const char* path, unsigned int mode) {
	(void)mode;
	return fs_create_directory_at_path(path) ? 0 : -1;
}

// Provide standard streams
static struct __bk_file __stdin_obj, __stdout_obj, __stderr_obj;
FILE* stdin = &__stdin_obj;
FILE* stdout = &__stdout_obj;
FILE* stderr = &__stderr_obj;


