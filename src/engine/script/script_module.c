#include "script_module.h"

#include <tyran_engine/resource/id.h>
#include "event/script_object_updated.h"
#include <tyran_engine/event/resource_updated.h>

#include "../state/state_update.h"
#include <tyran_engine/module/modules.h>
#include <tyranscript/tyran_log.h>
#include <tyranscript/tyran_memory.h>

extern nimbus_modules* g_modules;

static void _on_update(void* _self)
{
	// nimbus_script_module* self = _self;

	// nimbus_state_update_send(&self->main_state.arrays, &self->update.event_write_stream);
}

static void boot_script(nimbus_script_module* self)
{
}


static void _on_resource_updated(void* _self, struct nimbus_event_read_stream* stream)
{
	nimbus_script_module* self = _self;
	nimbus_resource_updated* updated;

	nimbus_event_stream_read_type_pointer(stream, updated, nimbus_resource_updated);
	if (updated->resource_type_id == self->script_type_id || updated->resource_type_id == self->combine_script_type_id) {
	}
}

void nimbus_script_module_init(void* _self, struct tyran_memory* memory)
{
	nimbus_script_module* self = _self;
	self->modules = g_modules;
	self->memory = memory;
	nimbus_update_init(&self->update, memory, _on_update, self, "script module");
	self->script_buffer_size = 16 * 1024;
	self->script_buffer = TYRAN_MEMORY_ALLOC(memory, self->script_buffer_size, "Script buffer");

	boot_script(self);

	self->script_type_id = nimbus_resource_type_id_from_string("oes");
	self->combine_script_type_id = nimbus_resource_type_id_from_string("oec");
	nimbus_event_listener_listen(&self->update.event_listener, NIMBUS_EVENT_RESOURCE_UPDATED, _on_resource_updated);
}
