#ifndef nimbus_dependency_resolver_h
#define nimbus_dependency_resolver_h

#include "../resource/resource_cache.h"
#include <tyran_engine/resource/id.h>
#include <tyran_engine/resource/type_id.h>
#include "resource_dependency_info.h"
#include <tyranscript/tyran_symbol.h>

struct tyran_symbol_table;
struct tyran_value;
struct nimbus_event_write_stream;
struct tyran_memory;

typedef struct nimbus_dependency_resolver {
	struct tyran_symbol_table* symbol_table;
	int dependency_info_count;
	nimbus_resource_dependency_info dependency_infos[64];
	int dependency_info_max_count;
	nimbus_resource_id loading_resources[64];
	int loading_resources_max_count;
	int loading_resources_count;
	nimbus_resource_cache resource_cache;
	struct nimbus_event_write_stream* event_write_stream;
	nimbus_resource_type_id object_type_id;
	nimbus_resource_type_id wire_object_type_id;
	struct tyran_memory* memory;
	tyran_symbol type_symbol;
} nimbus_dependency_resolver;

void nimbus_dependency_resolver_init(nimbus_dependency_resolver* self, struct tyran_memory* memory, struct tyran_symbol_table* symbol_table, struct nimbus_event_write_stream* stream);
void nimbus_dependency_resolver_object_loaded(nimbus_dependency_resolver* self, struct tyran_object* v, nimbus_resource_id resource_id, nimbus_resource_type_id resource_type_id);

#endif
