#ifndef sinkblobs_object_listeners_h
#define sinkblobs_object_listeners_h

#include "../../core/src/base/update/nimbus_update.h"
#include "../resource/resource_type_id.h"

struct nimbus_object_listener_info;
struct tyran_memory;

typedef struct nimbus_object_listener {
	struct nimbus_object_listener_info* infos;
	int info_count;
	int info_max_count;
	struct tyran_symbol_table* symbol_table;
	nimbus_update update;
	nimbus_resource_type_id object_type_id;
} nimbus_object_listener;



void nimbus_object_listener_init(nimbus_object_listener* self, struct tyran_memory* memory, struct tyran_symbol_table* table);

#endif
