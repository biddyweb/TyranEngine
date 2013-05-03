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

nimbus_resource_id nimbus_resource_handler_calculate_resource_id(const char* name)
{
	const char* p;
	u32t h = 0;
	u32t g = 0;

	for(p = name; *p != '\0'; p = p + 1) {
		h = (h << 4) + *p;

		if ((g = h & 0xf0000000) != 0) {
			h = h ^ (g >> 24);
			h = h ^ g;
		}
	}

	return (nimbus_resource_id)h;
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
		id = self->resource_infos_count++;
		nimbus_resource_info* info = &self->resource_infos[id];
		info->resource_id = nimbus_resource_handler_calculate_resource_id(name);
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
