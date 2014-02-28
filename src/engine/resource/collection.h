#ifndef nimbus_resource_collection_h
#define nimbus_resource_collection_h

#include <tyranscript/tyran_types.h>
#include <tyran_engine/resource/id.h>
#include <tyran_engine/resource/type_id.h>

struct tyran_memory;
struct nimbus_resource;

typedef struct nimbus_resource_collection {
	u8t* resources;
	int resource_count;
	int max_resource_count;
	size_t struct_size;
	nimbus_resource_type_id type_id;
} nimbus_resource_collection;

void nimbus_resource_collection_init(nimbus_resource_collection* self, nimbus_resource_type_id type_id, struct tyran_memory* memory, size_t struct_size, int max_resource_count);
void nimbus_resource_collection_destroy(nimbus_resource_collection* self);
struct nimbus_resource* nimbus_resource_collection_add(nimbus_resource_collection* self, nimbus_resource_id resource_id);
const struct nimbus_resource* nimbus_resource_collection_find(const nimbus_resource_collection* self, nimbus_resource_id resource_id);
struct nimbus_resource* nimbus_resource_collection_find_or_create(nimbus_resource_collection* self, nimbus_resource_id resource_id);

#endif
