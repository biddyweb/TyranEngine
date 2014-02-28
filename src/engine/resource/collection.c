#include "collection.h"

#include <tyranscript/tyran_types.h>
#include <tyranscript/tyran_memory.h>
#include <tyranscript/tyran_log.h>

#include "resource.h"

void nimbus_resource_collection_init(nimbus_resource_collection* self, nimbus_resource_type_id type_id, struct tyran_memory* memory, size_t struct_size, int max_resource_count)
{
	self->max_resource_count = max_resource_count;
	self->struct_size = struct_size;
	self->resources = TYRAN_MEMORY_CALLOC_TYPE_COUNT(memory, u8t, max_resource_count);
	self->resource_count = 0;
	self->type_id = type_id;
}

void nimbus_resource_collection_destroy(nimbus_resource_collection* self)
{
	self->resource_count = 0;
	TYRAN_MEMORY_FREE(self->resources);
}

nimbus_resource* nimbus_resource_collection_add(nimbus_resource_collection* self, nimbus_resource_id resource_id)
{
	TYRAN_ASSERT(self->resource_count < self->max_resource_count, "Too many resource");
	nimbus_resource* resource = (nimbus_resource*) &self->resources[self->struct_size * self->resource_count++];
	return resource;
}
