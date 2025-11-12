#include "math.h"

double fabs(double x) { return x < 0 ? -x : x; }
double floor(double x) { long i = (long)x; return (x < 0 && (double)i != x) ? (double)(i - 1) : (double)i; }
double ceil(double x) { long i = (long)x; return (x > 0 && (double)i != x) ? (double)(i + 1) : (double)i; }
double sqrt(double x) {
	if (x <= 0) return 0;
	double guess = x / 2.0;
	for (int i = 0; i < 16; ++i) {
		guess = 0.5 * (guess + x / guess);
	}
	return guess;
}

float fabsf(float x) { return x < 0 ? -x : x; }
float floorf(float x) { int i = (int)x; return (x < 0 && (float)i != x) ? (float)(i - 1) : (float)i; }
float ceilf(float x) { int i = (int)x; return (x > 0 && (float)i != x) ? (float)(i + 1) : (float)i; }
float sqrtf(float x) {
	if (x <= 0) return 0;
	float guess = x / 2.0f;
	for (int i = 0; i < 12; ++i) {
		guess = 0.5f * (guess + x / guess);
	}
	return guess;
}


