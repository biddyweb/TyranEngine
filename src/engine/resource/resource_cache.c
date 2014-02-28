#include "resource_cache.h"
#include <tyranscript/tyran_memory.h>
#include <tyranscript/tyran_value.h>
#include <tyran_engine/resource/resource.h>
#include <tyran_engine/resource/type_id.h>

void nimbus_resource_cache_init(nimbus_resource_cache* self, struct tyran_memory* memory)
{
	self->max_entries_count = 256;
	self->entries = TYRAN_MEMORY_CALLOC_TYPE_COUNT(memory, nimbus_resource_cache_entry, self->max_entries_count);
	self->entries_count = 0;
}

void nimbus_resource_cache_free(nimbus_resource_cache* self)
{
	TYRAN_MEMORY_FREE(self->entries);
}

void nimbus_resource_cache_add(nimbus_resource_cache* self, nimbus_resource_id resource_id, const nimbus_resource* value)
{
	TYRAN_ASSERT(self->entries_count < self->max_entries_count, "Resource cache out of space");
	nimbus_resource_cache_entry* entry = &self->entries[self->entries_count];
	self->entries_count++;
	entry->resource_id = resource_id;
	entry->resource = value;
}

const nimbus_resource* nimbus_resource_cache_find(nimbus_resource_cache* self, nimbus_resource_id resource_id) {
	for (int i=0; i<self->entries_count; ++i) {
		if (self->entries[i].resource_id == resource_id) {
			return self->entries[i].resource;
		}
	}

	return 0;
}

const nimbus_resource* nimbus_resource_cache_create(nimbus_resource_cache* self, nimbus_resource_id resource_id, nimbus_resource_type_id type_id)
{
	return 0;
}

const nimbus_resource* nimbus_resource_cache_find_or_create(nimbus_resource_cache* self, nimbus_resource_id resource_id, nimbus_resource_type_id type_id) {
	const nimbus_resource* resource = nimbus_resource_cache_find(self, resource_id);
	if (!resource) {
		resource = nimbus_resource_cache_create(self, resource_id, type_id);
	}
	
	return resource;
}

const nimbus_resource* nimbus_resource_cache_get(nimbus_resource_cache* self, nimbus_resource_id resource_id) {
	const nimbus_resource* v = nimbus_resource_cache_find(self, resource_id);
	TYRAN_ASSERT(v != 0, "get_resource requires an existing resource:%d", resource_id);
	return v;
}
