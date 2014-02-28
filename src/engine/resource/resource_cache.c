#include "resource_cache.h"
#include <tyranscript/tyran_memory.h>
#include <tyranscript/tyran_value.h>
#include <tyran_engine/resource/resource.h>
#include <tyran_engine/resource/type_id.h>
#include <tyran_engine/resource/resource_definition.h>
#include "collection.h"

void nimbus_resource_cache_init(nimbus_resource_cache* self, struct tyran_memory* memory)
{
	self->max_collections_count = 256;
	self->collections = TYRAN_MEMORY_CALLOC_TYPE_COUNT(memory, nimbus_resource_collection, self->max_collections_count);
	self->collections_count = 0;
	self->memory = memory;

	self->max_resources_count = 512;
	self->resources_count = 0;
	self->resources = TYRAN_MEMORY_CALLOC_TYPE_COUNT(memory, nimbus_resource*, self->max_resources_count);
}

void nimbus_resource_cache_free(nimbus_resource_cache* self)
{
	TYRAN_MEMORY_FREE(self->collections);
	TYRAN_MEMORY_FREE(self->resources);
}

void nimbus_resource_cache_add(nimbus_resource_cache* self, const nimbus_resource_definition* definition)
{
	TYRAN_ASSERT(self->collections_count < self->max_collections_count, "Resource cache out of space");
	nimbus_resource_collection* collection = &self->collections[self->collections_count++];
	nimbus_resource_collection_init(collection, definition->resource_type_id, self->memory, definition->struct_size, 256);
}

const nimbus_resource* nimbus_resource_cache_find(nimbus_resource_cache* self, nimbus_resource_id resource_id) {
	for (int i=0; i<self->resources_count; ++i) {
		if (self->resources[i]->id == resource_id) {
			return self->resources[i];
		}
	}

	return 0;
}

nimbus_resource_collection* resource_collection_from_type(nimbus_resource_cache* self, nimbus_resource_type_id type_id)
{
	for (int i=0; i<self->collections_count; ++i) {
		nimbus_resource_collection* collection = &self->collections[i];
		if (collection->type_id == type_id) {
			return collection;
		}
	}
	TYRAN_ERROR("couldn't find collection for type:%d", type_id);
	return 0;
}

const nimbus_resource* nimbus_resource_cache_create(nimbus_resource_cache* self, nimbus_resource_id resource_id, nimbus_resource_type_id type_id)
{
	nimbus_resource_collection* collection = resource_collection_from_type(self, type_id);
	nimbus_resource* resource = nimbus_resource_collection_add(collection, resource_id);
	self->resources[self->resources_count++] = resource;

	return resource;
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
