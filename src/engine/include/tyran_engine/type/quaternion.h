#ifndef nimbus_quaternion_h
#define nimbus_quaternion_h

#include "vector3.h"

typedef struct nimbus_quaternion {
	nimbus_vector3 v;
	tyran_number w;
} nimbus_quaternion;

void nimbus_quaternion_from_euler(nimbus_quaternion* q, float roll, float pitch, float yaw);

#endif
