#include "combine_module.h"

#include <tyran_engine/resource/id.h>
#include <tyran_engine/event/resource_updated.h>

#include "../state/state_update.h"
#include <tyran_engine/state/combine.h>
#include <tyran_engine/module/modules.h>
#include <tyranscript/tyran_log.h>
#include <tyranscript/tyran_memory.h>
#include "../yaml/yaml_converter.h"

extern nimbus_modules* g_modules;

static void _on_update(void* _self)
{
	nimbus_combine_module* self = _self;

	nimbus_state_update_send(&self->main_state.arrays, &self->update.event_write_stream);
}

static void boot_script(nimbus_combine_module* self)
{
}

static void on_combine(const nimbus_resource_updated* updated, struct nimbus_event_read_stream* stream)
{

}

static void parse_yaml(nimbus_combine_module* self)
{
	nimbus_yaml_converter converter;
	nimbus_combine combine;
	nimbus_combine_init(&combine, &self->main_state);
	nimbus_yaml_converter_init(&converter, g_modules->symbol_table, g_modules->component_definitions, g_modules->component_definitions_count, &combine);

	nimbus_yaml_converter_parse(&converter, (const char*)self->script_buffer, self->script_buffer_count);
}

static void read_stream(nimbus_combine_module* self, struct nimbus_event_read_stream* stream, nimbus_resource_id resource_id, nimbus_resource_type_id resource_type_id, int payload_size)
{
	TYRAN_ASSERT(payload_size <= self->script_buffer_size, "Buffer too small for script. payload:%d max:%d", payload_size, self->script_buffer_size);

	nimbus_event_stream_read_octets(stream, self->script_buffer, payload_size);
	self->script_buffer[payload_size] = 0;
	self->script_buffer_count = payload_size;

	TYRAN_LOG("*** EVALUATE *** %d octet_size:%d", resource_id, payload_size);
	TYRAN_LOG("SCRIPT:'%s'", self->script_buffer);
}

static void _on_resource_updated(void* _self, struct nimbus_event_read_stream* stream)
{
	nimbus_combine_module* self = _self;
	nimbus_resource_updated* updated;

	nimbus_event_stream_read_type_pointer(stream, updated, nimbus_resource_updated);
	if (updated->resource_type_id == self->combine_script_type_id) {
		read_stream(self, stream, updated->resource_id, updated->resource_type_id, updated->payload_size);
		parse_yaml(self);
		on_combine(updated, stream);
	}
}

void nimbus_combine_module_init(void* _self, struct tyran_memory* memory)
{
	nimbus_combine_module* self = _self;
	self->modules = g_modules;
	self->memory = memory;
	nimbus_update_init(&self->update, memory, _on_update, self, "combine module");
	self->script_buffer_size = 16 * 1024;
	self->script_buffer = TYRAN_MEMORY_ALLOC(memory, self->script_buffer_size, "Script buffer");

	boot_script(self);

	nimbus_state_init(&self->main_state, memory, g_modules->component_definitions, g_modules->component_definitions_count);


	self->combine_script_type_id = nimbus_resource_type_id_from_string("oec");
	nimbus_event_listener_listen(&self->update.event_listener, NIMBUS_EVENT_RESOURCE_UPDATED, _on_resource_updated);
}
