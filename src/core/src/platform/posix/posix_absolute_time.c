#include "posix_absolute_time.h"
#include <stdint.h>

void nimbus_absolute_time_init(nimbus_absolute_time* a)
{
	clock_gettime(CLOCK_MONOTONIC, &a->absolute);
}

static uint64_t nano_seconds(const struct timespec* t)
{
  return t->tv_sec * 10e9 + t->tv_nsec;
}

double nimbus_absolute_time_delta(const nimbus_absolute_time* a, const nimbus_absolute_time* b)
{
  uint64_t nanoseconds = nano_seconds(&a->absolute) - nano_seconds(&b->absolute);
	double seconds = nanoseconds / 1000000000.0;
	return seconds;
}
