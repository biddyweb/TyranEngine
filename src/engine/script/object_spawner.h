#ifndef nimbus_object_spawner_h
#define nimbus_object_spawner_h

#include <tyranscript/tyran_symbol.h>

struct tyran_object;
struct nimbus_event_definition;

typedef struct nimbus_object_spawner_association {
	struct tyran_object* destination_component;
	struct tyran_object* source_component;
} nimbus_object_spawner_association;

typedef struct nimbus_object_spawner {
	struct tyran_runtime* runtime;
	struct tyran_object* object_to_spawn;
	struct tyran_symbol_table* symbol_table;
	nimbus_object_spawner_association associations[64];
	int associations_count;
	int associations_max_count;
	struct nimbus_event_definition* defintions;
	int definitions_count;
	tyran_symbol type_symbol;
} nimbus_object_spawner;

void nimbus_object_spawner_init(nimbus_object_spawner* self, struct tyran_runtime* runtime, struct nimbus_event_definition* defintions, int definitions_count, struct tyran_object* object_to_spawn);
struct tyran_object* nimbus_object_spawner_spawn(nimbus_object_spawner* self);

#endif
