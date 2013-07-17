#include <tyran_engine/script/object_decorator.h>

#include "object_info.h"
#include <tyranscript/tyran_object.h>

nimbus_object_info* nimbus_decorate_object(struct tyran_object* o, struct tyran_memory* memory)
{
	TYRAN_LOG("decorate");
	nimbus_object_info* info = TYRAN_MEMORY_CALLOC_TYPE(memory, nimbus_object_info);
	info->instance_id = 0; // self->instance_id++;
	info->track_index = -1;
	info->is_module_resource = TYRAN_FALSE;

	void* current = tyran_object_program_specific(o);
	TYRAN_ASSERT(current == 0, "Current must be null");
	tyran_object_set_program_specific(o, info);

	return info;
}
