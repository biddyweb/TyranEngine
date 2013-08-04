#ifndef nimbus_resource_cache_h
#define nimbus_resource_cache_h

struct tyran_value;
struct tyran_memory;

#include "id.h"

typedef struct nimbus_resource_cache_entry {
	nimbus_resource_id resource_id;
	struct tyran_object* value;
} nimbus_resource_cache_entry;

typedef struct nimbus_resource_cache {
	nimbus_resource_cache_entry* entries;
	int entries_count;
	int max_entries_count;
} nimbus_resource_cache;


void nimbus_resource_cache_init(nimbus_resource_cache* self, struct tyran_memory* memory);
void nimbus_resource_cache_free(nimbus_resource_cache* self);

void nimbus_resource_cache_add(nimbus_resource_cache* self, nimbus_resource_id resource_id, struct tyran_object* value);
struct tyran_object* nimbus_resource_cache_find(nimbus_resource_cache* self, nimbus_resource_id resource_id);
struct tyran_object* nimbus_resource_cache_get(nimbus_resource_cache* self, nimbus_resource_id resource_id);

#endif
