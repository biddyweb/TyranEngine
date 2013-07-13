#ifndef sinkblobs_object_listeners_h
#define sinkblobs_object_listeners_h

#include <tyran_core/update/update.h>
#include <tyran_engine/resource/type_id.h>
#include <tyranscript/tyran_symbol.h>
#include <tyranscript/tyran_value.h>

struct tyran_memory;

typedef struct nimbus_object_listener_function {
	tyran_value function_context;
	const struct tyran_function* function;
} nimbus_object_listener_function;

typedef struct nimbus_object_listener_info {
	tyran_symbol symbol;
	nimbus_object_listener_function functions[64];
	int function_count;
	int max_function_count;
} nimbus_object_listener_info;


typedef struct nimbus_object_listener {
	struct nimbus_object_listener_info infos[64];
	int info_count;
	int info_max_count;
	struct tyran_symbol_table* symbol_table;
	nimbus_update update;
	nimbus_resource_type_id state_type_id;
	tyran_symbol frame_symbol;
	struct tyran_runtime* runtime;
} nimbus_object_listener;


void nimbus_object_listener_init(nimbus_object_listener* self, struct tyran_memory* memory, struct tyran_runtime* runtime);

#endif
