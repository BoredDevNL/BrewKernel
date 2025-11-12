#include "stdlib.h"
#include "string.h"
#include <stdint.h>

int atoi(const char* s) {
	return (int)strtol(s, 0, 10);
}

long atol(const char* s) {
	return strtol(s, 0, 10);
}

static int isspace_c(int c) {
	return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
}
static int isdigit_c(int c) { return c >= '0' && c <= '9'; }
static int isalpha_c(int c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }
static int tolower_c(int c) { if (c >= 'A' && c <= 'Z') return c - 'A' + 'a'; return c; }

long strtol(const char* nptr, char** endptr, int base) {
	const char* s = nptr;
	long sign = 1;
	long result = 0;

	while (isspace_c(*s)) ++s;
	if (*s == '+' || *s == '-') { if (*s == '-') sign = -1; ++s; }

	if (base == 0) {
		if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) { base = 16; s += 2; }
		else if (s[0] == '0') { base = 8; s += 1; }
		else base = 10;
	} else if (base == 16) {
		if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) s += 2;
	}

	while (*s) {
		int v;
		if (isdigit_c(*s)) v = *s - '0';
		else if (isalpha_c(*s)) v = tolower_c(*s) - 'a' + 10;
		else break;
		if (v >= base) break;
		result = result * base + v;
		++s;
	}
	if (endptr) *endptr = (char*)s;
	return result * sign;
}

unsigned long strtoul(const char* nptr, char** endptr, int base) {
	long v = strtol(nptr, endptr, base);
	if (v < 0) return 0;
	return (unsigned long)v;
}

int abs(int x) { return x < 0 ? -x : x; }
double atof(const char* s) {
	// Minimal: parse integer and fractional part
	if (!s) return 0.0;
	int sign = 1;
	while (isspace_c(*s)) ++s;
	if (*s == '+' || *s == '-') { if (*s == '-') sign = -1; ++s; }
	long whole = 0;
	while (isdigit_c(*s)) { whole = whole * 10 + (*s - '0'); ++s; }
	double value = (double)whole;
	if (*s == '.') {
		++s;
		double place = 0.1;
		while (isdigit_c(*s)) {
			value += (*s - '0') * place;
			place *= 0.1;
			++s;
		}
	}
	return sign * value;
}

static uint32_t lcg_state = 1;
void srand(unsigned int seed) { lcg_state = seed ? seed : 1; }
int rand(void) {
	// Simple LCG
	lcg_state = (1103515245u * lcg_state + 12345u);
	return (int)((lcg_state >> 16) & 0x7FFF);
}

void qsort(void* base, size_t nmemb, size_t size,
           int (*compar)(const void*, const void*)) {
	// Simple insertion sort to keep small and freestanding
	uint8_t* b = (uint8_t*)base;
	for (size_t i = 1; i < nmemb; ++i) {
		size_t j = i;
		while (j > 0) {
			uint8_t* a_ptr = b + (j - 1) * size;
			uint8_t* c_ptr = b + j * size;
			if (compar(a_ptr, c_ptr) <= 0) break;
			// swap
			for (size_t k = 0; k < size; ++k) {
				uint8_t tmp = a_ptr[k];
				a_ptr[k] = c_ptr[k];
				c_ptr[k] = tmp;
			}
			--j;
		}
	}
}

char* getenv(const char* name) { (void)name; return 0; }
int putenv(char* string) { (void)string; return 0; }
int system(const char* command) { (void)command; return -1; }


