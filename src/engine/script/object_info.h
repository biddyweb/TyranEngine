#ifndef nimbus_object_info_h
#define nimbus_object_info_h

#include <tyranscript/tyran_symbol.h>
#include <tyran_engine/resource/id.h>

typedef struct nimbus_object_info_layer {
	nimbus_resource_id resource_id;
	tyran_object* object;
} nimbus_object_info_layer;

typedef struct nimbus_object_info {
	tyran_symbol symbol;
	int instance_id;
	nimbus_object_info_layer layers[8];
	int layers_count;
} nimbus_object_info;

#endif
