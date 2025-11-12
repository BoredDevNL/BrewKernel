#include "string.h"
#include "../../print.h"
#include <stdint.h>
#include <stddef.h>
#include "stdlib.h"

void* memcpy(void* dest, const void* src, size_t n) {
	uint8_t* d = (uint8_t*)dest;
	const uint8_t* s = (const uint8_t*)src;
	for (size_t i = 0; i < n; ++i) d[i] = s[i];
	return dest;
}

void* memmove(void* dest, const void* src, size_t n) {
	uint8_t* d = (uint8_t*)dest;
	const uint8_t* s = (const uint8_t*)src;
	if (d == s || n == 0) return dest;
	if (d < s) {
		for (size_t i = 0; i < n; ++i) d[i] = s[i];
	} else {
		for (size_t i = n; i-- > 0;) d[i] = s[i];
	}
	return dest;
}

void* memset(void* s, int c, size_t n) {
	uint8_t* p = (uint8_t*)s;
	uint8_t v = (uint8_t)c;
	for (size_t i = 0; i < n; ++i) p[i] = v;
	return s;
}

int memcmp(const void* s1, const void* s2, size_t n) {
	const uint8_t* a = (const uint8_t*)s1;
	const uint8_t* b = (const uint8_t*)s2;
	for (size_t i = 0; i < n; ++i) {
		if (a[i] != b[i]) return (int)a[i] - (int)b[i];
	}
	return 0;
}

size_t strlen(const char* s) {
	size_t i = 0;
	while (s[i] != 0) ++i;
	return i;
}

char* strcpy(char* dest, const char* src) {
	char* d = dest;
	while ((*d++ = *src++) != 0) {}
	return dest;
}

char* strncpy(char* dest, const char* src, size_t n) {
	size_t i = 0;
	for (; i < n && src[i]; ++i) dest[i] = src[i];
	for (; i < n; ++i) dest[i] = '\0';
	return dest;
}

int strcmp(const char* s1, const char* s2) {
	while (*s1 && (*s1 == *s2)) { ++s1; ++s2; }
	return (unsigned char)*s1 - (unsigned char)*s2;
}

int strncmp(const char* s1, const char* s2, size_t n) {
	for (size_t i = 0; i < n; ++i) {
		if (s1[i] != s2[i] || s1[i] == '\0' || s2[i] == '\0') {
			return (unsigned char)s1[i] - (unsigned char)s2[i];
		}
	}
	return 0;
}

static char tolower_ch(char c) {
	if (c >= 'A' && c <= 'Z') return (char)(c - 'A' + 'a');
	return c;
}

int strcasecmp(const char* s1, const char* s2) {
	while (*s1 && *s2) {
		char a = tolower_ch(*s1);
		char b = tolower_ch(*s2);
		if (a != b) return (unsigned char)a - (unsigned char)b;
		++s1; ++s2;
	}
	return (unsigned char)tolower_ch(*s1) - (unsigned char)tolower_ch(*s2);
}

int strncasecmp(const char* s1, const char* s2, size_t n) {
	for (size_t i = 0; i < n; ++i) {
		char a = tolower_ch(s1[i]);
		char b = tolower_ch(s2[i]);
		if (a != b || a == '\0' || b == '\0') {
			return (unsigned char)a - (unsigned char)b;
		}
	}
	return 0;
}

char* strchr(const char* s, int c) {
	char ch = (char)c;
	for (; *s; ++s) if (*s == ch) return (char*)s;
	return (ch == 0) ? (char*)s : 0;
}

char* strrchr(const char* s, int c) {
	const char* last = 0;
	char ch = (char)c;
	for (; *s; ++s) if (*s == ch) last = s;
	return (char*)(ch == 0 ? s : last);
}

char* strstr(const char* haystack, const char* needle) {
	if (!*needle) return (char*)haystack;
	size_t nl = strlen(needle);
	for (; *haystack; ++haystack) {
		if (*haystack == *needle) {
			if (strncmp(haystack, needle, nl) == 0) return (char*)haystack;
		}
	}
	return 0;
}

char* strdup(const char* s) {
	size_t n = strlen(s) + 1;
	char* d = (char*)malloc(n);
	if (!d) return 0;
	memcpy(d, s, n);
	return d;
}


