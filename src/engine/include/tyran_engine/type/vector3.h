#ifndef nimbus_vector3_h
#define nimbus_vector3_h

#include <tyranscript/tyran_types.h>

typedef struct nimbus_vector3 {
	tyran_number x;
	tyran_number y;
	tyran_number z;
} nimbus_vector3;

nimbus_vector3 nimbus_vector3_make(tyran_number x, tyran_number y, tyran_number z);




#endif
