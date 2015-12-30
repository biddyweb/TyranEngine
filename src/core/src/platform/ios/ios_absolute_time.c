#include "ios_absolute_time.h"


void nimbus_absolute_time_init(nimbus_absolute_time* a)
{
	a->absolute = mach_absolute_time();
}

double nimbus_absolute_time_delta(const nimbus_absolute_time* a, const nimbus_absolute_time* b)
{
    static mach_timebase_info_data_t s_timebase_info;
    
	uint64_t elapsed = a->absolute - b->absolute;

    if ( s_timebase_info.denom == 0 ) {
        mach_timebase_info(&s_timebase_info);
    }
    
    uint64_t nanoseconds = elapsed * s_timebase_info.numer / s_timebase_info.denom;
    
    double seconds = nanoseconds / 1000000000.0;

    
    return seconds;
}
