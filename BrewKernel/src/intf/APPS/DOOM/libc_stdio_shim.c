/*
 * Minimal stdio shim: forward output to BrewKernel print routines.
 */
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include "print.h"
#include "stdio.h"

int putchar(int c) {
	print_char((char)c);
	return c;
}

int puts(const char* s) {
	if (!s) return 0;
	brew_str(s);
	print_char('\n');
	return 1;
}

static void print_uint_hex(unsigned int v) {
	const char* hex = "0123456789abcdef";
	char buf[9];
	for (int i = 7; i >= 0; --i) {
		buf[i] = hex[v & 0xF];
		v >>= 4;
	}
	for (int i = 0; i < 8; ++i) print_char(buf[i]);
}

static void print_int_dec(int v) {
	brew_int(v);
}

static void vprint_basic(const char* fmt, va_list ap) {
	for (const char* p = fmt; *p; ++p) {
		if (*p != '%') {
			print_char(*p);
			continue;
		}
		++p;
		if (*p == '\0') break;
		switch (*p) {
			case 's': {
				const char* s = va_arg(ap, const char*);
				if (s) brew_str(s);
				break;
			}
			case 'd':
			case 'i': {
				int v = va_arg(ap, int);
				print_int_dec(v);
				break;
			}
			case 'u': {
				unsigned int v = va_arg(ap, unsigned int);
				print_uint(v);
				break;
			}
			case 'x':
			case 'X': {
				unsigned int v = va_arg(ap, unsigned int);
				print_uint_hex(v);
				break;
			}
			case 'c': {
				int ch = va_arg(ap, int);
				print_char((char)ch);
				break;
			}
			case '%': {
				print_char('%');
				break;
			}
			default: {
				// Unknown specifier: print it literally
				print_char('%');
				print_char(*p);
				break;
			}
		}
	}
}

int printf(const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vprint_basic(fmt, ap);
	va_end(ap);
	return 0;
}

static int buffer_putc(char** dst, size_t* rem, char c) {
	if (*rem > 1) {
		**dst = c;
		(*dst)++;
		(*rem)--;
		return 1;
	}
	return 0;
}

static int buffer_write_str(char** dst, size_t* rem, const char* s) {
	int written = 0;
	while (*s) {
		if (!buffer_putc(dst, rem, *s)) break;
		++s; ++written;
	}
	return written;
}

static int buffer_write_uint(char** dst, size_t* rem, unsigned int v) {
	char tmp[16];
	int pos = 0;
	if (v == 0) {
		return buffer_putc(dst, rem, '0');
	}
	while (v > 0 && pos < (int)sizeof(tmp)) {
		tmp[pos++] = '0' + (v % 10);
		v /= 10;
	}
	int written = 0;
	while (pos > 0) {
		if (!buffer_putc(dst, rem, tmp[--pos])) break;
		++written;
	}
	return written;
}

static int buffer_write_int(char** dst, size_t* rem, int v) {
	if (v < 0) {
		buffer_putc(dst, rem, '-');
		return 1 + buffer_write_uint(dst, rem, (unsigned int)(-v));
	}
	return buffer_write_uint(dst, rem, (unsigned int)v);
}

int vsnprintf(char* buf, size_t buf_len, const char* fmt, va_list ap) {
	if (buf_len == 0) return 0;
	char* dst = buf;
	size_t rem = buf_len;
	const char* p = fmt;
	int count = 0;
	while (*p) {
		if (*p != '%') {
			if (buffer_putc(&dst, &rem, *p)) ++count;
			++p;
			continue;
		}
		++p;
		if (*p == '\0') break;
		switch (*p) {
			case 's': {
				const char* s = va_arg(ap, const char*);
				if (s) count += buffer_write_str(&dst, &rem, s);
				break;
			}
			case 'd':
			case 'i': {
				int v = va_arg(ap, int);
				count += buffer_write_int(&dst, &rem, v);
				break;
			}
			case 'u': {
				unsigned int v = va_arg(ap, unsigned int);
				count += buffer_write_uint(&dst, &rem, v);
				break;
			}
			case 'c': {
				int ch = va_arg(ap, int);
				if (buffer_putc(&dst, &rem, (char)ch)) ++count;
				break;
			}
			case '%': {
				if (buffer_putc(&dst, &rem, '%')) ++count;
				break;
			}
			default: {
				// Unknown specifier: write literally
				if (buffer_putc(&dst, &rem, '%')) ++count;
				if (buffer_putc(&dst, &rem, *p)) ++count;
				break;
			}
		}
		++p;
	}
	// null-terminate
	*dst = '\0';
	return count;
}

int snprintf(char* buf, size_t buf_len, const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int r = vsnprintf(buf, buf_len, fmt, ap);
	va_end(ap);
	return r;
}

int fprintf(FILE* stream, const char* fmt, ...) {
	(void)stream;
	va_list ap;
	va_start(ap, fmt);
	vprint_basic(fmt, ap);
	va_end(ap);
	return 0;
}

int vfprintf(FILE* stream, const char* fmt, va_list ap) {
	(void)stream;
	vprint_basic(fmt, ap);
	return 0;
}

void abort(void) {
	brew_str("\n[abort]\n");
	while (1) {}
}

void exit(int code) {
	(void)code;
	brew_str("\n[exit]\n");
	// Return to caller; nothing else to do in freestanding kernel
}


