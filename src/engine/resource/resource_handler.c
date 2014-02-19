#include "resource_handler.h"

#include <tyranscript/tyran_memory.h>
#include <tyranscript/tyran_clib.h>

nimbus_resource_handler* nimbus_resource_handler_new(tyran_memory* memory)
{
	nimbus_resource_handler* self = TYRAN_MEMORY_CALLOC_TYPE(memory, nimbus_resource_handler);
	self->resource_infos_max_count = 1000;
	self->resource_infos = TYRAN_MEMORY_CALLOC_TYPE_COUNT(memory, nimbus_resource_info, self->resource_infos_max_count);
	self->memory = memory;

	return self;
}

nimbus_resource_id nimbus_resource_handler_find_name(nimbus_resource_handler* self, const char* name)
{
	for (int i=0; i<self->resource_infos_count; ++i) {
		if (tyran_strcmp(self->resource_infos[i].filename, name) == 0) {
			return self->resource_infos[i].resource_id;
		}
	}
	return 0;
}

nimbus_resource_id nimbus_resource_handler_add(nimbus_resource_handler* self, const char* name)
{
	nimbus_resource_id id = nimbus_resource_handler_find_name(self, name);
	if (!id) {
		int index = self->resource_infos_count++;
		nimbus_resource_info* info = &self->resource_infos[index];
		id = nimbus_resource_id_from_string(name);
		info->resource_id = id;
		info->filename = tyran_str_dup(self->memory, name);
	}

	return id;
}

const char* nimbus_resource_handler_id_to_name(nimbus_resource_handler* self, nimbus_resource_id id)
{
	for (int i=0; i<self->resource_infos_count; ++i) {
		if (self->resource_infos[i].resource_id == id) {
			return self->resource_infos[i].filename;
		}
	}
	return 0;
}
