#ifndef sinkblobs_object_listeners_h
#define sinkblobs_object_listeners_h

#include <tyran_core/update/update.h>
#include <tyran_engine/resource/type_id.h>
#include <tyranscript/tyran_symbol.h>
#include <tyranscript/tyran_value.h>
#include "object_to_event.h"

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

typedef struct nimbus_object_layer {
	char* name;
} nimbus_object_layer;

typedef struct nimbus_object_collection {
	struct tyran_object** entries;
	int count;
	int max_count;
} nimbus_object_collection;



typedef struct nimbus_object_collection_for_type {
	nimbus_object_collection collection;
	struct nimbus_event_definition* event_definition;
} nimbus_object_collection_for_type;


typedef struct nimbus_object_listener {
	struct nimbus_object_listener_info infos[64];
	int info_count;
	int info_max_count;
	struct tyran_symbol_table* symbol_table;
	nimbus_update update;
	nimbus_resource_type_id state_type_id;
	tyran_symbol frame_symbol;
	struct tyran_runtime* runtime;
	tyran_symbol type_symbol;
	struct tyran_memory* memory;
	nimbus_object_layer* layers;
	int max_layers_count;
	int layers_count;
	nimbus_object_to_event object_to_event;

	nimbus_object_collection_for_type object_collection_for_types[32];
	int object_collection_for_types_count;
} nimbus_object_listener;


void nimbus_object_listener_init(nimbus_object_listener* self, struct tyran_memory* memory, struct tyran_runtime* runtime, struct nimbus_event_definition* event_definitions, int event_definition_count);

#endif
