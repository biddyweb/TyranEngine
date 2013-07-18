#ifndef nimbus_object_spawner_h
#define nimbus_object_spawner_h

struct tyran_mocha_api;
struct tyran_object;

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
} nimbus_object_spawner;

void nimbus_object_spawner_init(nimbus_object_spawner* self, struct tyran_runtime* runtime, struct tyran_object* object_to_spawn);
struct tyran_object* nimbus_object_spawner_spawn(nimbus_object_spawner* self);

#endif
