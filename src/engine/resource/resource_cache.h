#ifndef nimbus_resource_cache_h
#define nimbus_resource_cache_h

struct tyran_value;
struct tyran_memory;
struct nimbus_resource;

#include <tyran_engine/resource/id.h>
#include <tyran_engine/resource/type_id.h>

typedef struct nimbus_resource_cache_entry {
	nimbus_resource_id resource_id;
	const struct nimbus_resource* resource;
} nimbus_resource_cache_entry;

typedef struct nimbus_resource_cache {
	nimbus_resource_cache_entry* entries;
	int entries_count;
	int max_entries_count;
} nimbus_resource_cache;


void nimbus_resource_cache_init(nimbus_resource_cache* self, struct tyran_memory* memory);
void nimbus_resource_cache_free(nimbus_resource_cache* self);

void nimbus_resource_cache_add(nimbus_resource_cache* self, nimbus_resource_id resource_id, const struct nimbus_resource* value);
const struct nimbus_resource* nimbus_resource_cache_find(nimbus_resource_cache* self, nimbus_resource_id resource_id);
const struct nimbus_resource* nimbus_resource_cache_find_or_create(nimbus_resource_cache* self, nimbus_resource_id resource_id, nimbus_resource_type_id type_id);
const struct nimbus_resource* nimbus_resource_cache_get(nimbus_resource_cache* self, nimbus_resource_id resource_id);

#endif
