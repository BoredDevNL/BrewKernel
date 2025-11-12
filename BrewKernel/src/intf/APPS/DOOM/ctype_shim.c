#include "ctype.h"

int toupper(int c) {
	if (c >= 'a' && c <= 'z') return c - 'a' + 'A';
	return c;
}
int tolower(int c) {
	if (c >= 'A' && c <= 'Z') return c - 'A' + 'a';
	return c;
}
int isspace(int c) {
	return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
}
int isdigit(int c) {
	return c >= '0' && c <= '9';
}


