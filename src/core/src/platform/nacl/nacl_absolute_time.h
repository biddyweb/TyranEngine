#ifndef nimbus_absolute_time_h
#define nimbus_absolute_time_h

typedef struct nimbus_absolute_time {
	double absolute;
} nimbus_absolute_time;

void nimbus_absolute_time_init(nimbus_absolute_time* a);
double nimbus_absolute_time_delta(const nimbus_absolute_time* a, const nimbus_absolute_time* b);

#endif
