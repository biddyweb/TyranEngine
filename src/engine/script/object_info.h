#ifndef nimbus_object_info_h
#define nimbus_object_info_h

#include <tyranscript/tyran_symbol.h>

typedef struct nimbus_object_info {
	tyran_symbol symbol;
	int instance_id;
} nimbus_object_info;

#endif
