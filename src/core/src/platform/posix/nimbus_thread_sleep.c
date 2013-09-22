#define _POSIX_C_SOURCE 199309
#include <time.h>
#include "../../base/thread/nimbus_thread_sleep.h"

void nimbus_thread_sleep(tyran_number seconds)
{
	struct timespec rm;
	const int NANO_MULTIPLIER = 1e9;

	rm.tv_sec = (size_t) seconds;
	tyran_number nano_seconds = (seconds - (tyran_number)(size_t)seconds) * (tyran_number) NANO_MULTIPLIER;
	rm.tv_nsec = nano_seconds;

	nanosleep(&rm, &rm);
}
