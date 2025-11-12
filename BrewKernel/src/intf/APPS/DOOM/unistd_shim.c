#include "unistd.h"
#include "../../timer.h"

int isatty(int fd) {
	(void)fd;
	return 0;
}

unsigned int sleep(unsigned int seconds) {
	timer_sleep_ms(seconds * 1000U);
	return 0;
}

int usleep(unsigned int usec) {
	timer_sleep_ms(usec / 1000U);
	return 0;
}


