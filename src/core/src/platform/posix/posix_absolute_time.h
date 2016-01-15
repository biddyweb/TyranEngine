#ifndef nimbus_posix_absolute_time_h
#define nimbus_posix_absolute_time_h

#define _XOPEN_SOURCE 600

#include <time.h>

typedef struct nimbus_absolute_time {
	struct timespec absolute;
} nimbus_absolute_time;

void nimbus_absolute_time_init(nimbus_absolute_time* a);
double nimbus_absolute_time_delta(const nimbus_absolute_time* a, const nimbus_absolute_time* b);

#endif
