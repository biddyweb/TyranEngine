#ifndef nimbus_resource_cache_h
#define nimbus_resource_cache_h

struct tyran_value;
struct tyran_memory;
struct nimbus_resource;
struct nimbus_resource_definition;

#include <tyran_engine/resource/id.h>
#include <tyran_engine/resource/type_id.h>
#include "collection.h"

typedef struct nimbus_resource_cache {
	nimbus_resource_collection* collections;
	int collections_count;
	int max_collections_count;
	struct tyran_memory* memory;

	struct nimbus_resource** resources;
	int max_resources_count;
	int resources_count;

} nimbus_resource_cache;


void nimbus_resource_cache_init(nimbus_resource_cache* self, struct tyran_memory* memory);
void nimbus_resource_cache_free(nimbus_resource_cache* self);

void nimbus_resource_cache_add_definition(nimbus_resource_cache* self, const struct nimbus_resource_definition* definition);
const struct nimbus_resource* nimbus_resource_cache_find(nimbus_resource_cache* self, nimbus_resource_id resource_id);
const struct nimbus_resource* nimbus_resource_cache_find_or_create(nimbus_resource_cache* self, nimbus_resource_id resource_id, nimbus_resource_type_id type_id);
const struct nimbus_resource* nimbus_resource_cache_get(nimbus_resource_cache* self, nimbus_resource_id resource_id);

#endif
