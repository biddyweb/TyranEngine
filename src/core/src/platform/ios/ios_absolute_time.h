#ifndef nimbus_absolute_time_ios_h
#define nimbus_absolute_time_ios_h

#include <mach/mach.h>
#include <mach/mach_time.h>


typedef struct nimbus_absolute_time {
	uint64_t absolute;
} nimbus_absolute_time;

void nimbus_absolute_time_init(nimbus_absolute_time* a);
double nimbus_absolute_time_delta(const nimbus_absolute_time* a, const nimbus_absolute_time* b);

#endif
