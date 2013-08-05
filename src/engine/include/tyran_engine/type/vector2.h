#ifndef nimbus_vector2_h
#define nimbus_vector2_h

#include <tyranscript/tyran_types.h>

typedef struct nimbus_vector2 {
	tyran_number x;
	tyran_number y;
} nimbus_vector2;

nimbus_vector2 nimbus_vector2_make(tyran_number a, tyran_number b); 

#endif
