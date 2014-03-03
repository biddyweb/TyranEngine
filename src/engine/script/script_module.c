#include "script_module.h"
#include "script_global.h"

#include <tyran_engine/resource/id.h>
#include "event/script_object_updated.h"
#include <tyran_engine/event/resource_updated.h>
#include "script_state_parser.h"
#include "state.h"

#include <tyran_engine/module/modules.h>

extern nimbus_modules* g_modules;

static void _on_update(void* _self)
{
	//nimbus_script_module* self = _self;
}

static void boot_script(nimbus_script_module* self)
{
	tyran_mocha_api_new(&self->mocha, 1024, g_modules->symbol_table);
	self->mocha.default_runtime->program_specific_context = self;

	nimbus_script_global_init(&self->script_global, &self->mocha);
}

static tyran_object* evaluate(nimbus_script_module* self, const char* data)
{
	tyran_value new_object = tyran_mocha_api_create_object(&self->mocha);
	tyran_object* object = tyran_value_mutable_object(&new_object);

	tyran_object_set_prototype(object, self->script_global.context);
	tyran_value temp_value;
	tyran_value_set_nil(temp_value);
	tyran_mocha_api_eval(&self->mocha, &new_object, &temp_value, data);
	tyran_runtime_clear(self->mocha.default_runtime);

	return tyran_value_mutable_object(&new_object);
}

static tyran_object* evaluate_stream(nimbus_script_module* self, struct nimbus_event_read_stream* stream, nimbus_resource_id resource_id, nimbus_resource_type_id resource_type_id, int payload_size)
{
	TYRAN_ASSERT(payload_size <= self->script_buffer_size, "Buffer too small for script. payload:%d max:%d", payload_size, self->script_buffer_size);

	nimbus_event_stream_read_octets(stream, self->script_buffer, payload_size);
	self->script_buffer[payload_size] = 0;

	TYRAN_LOG("*** EVALUATE *** %d octet_size:%d", resource_id, payload_size);
	TYRAN_LOG("SCRIPT:'%s'", self->script_buffer);
	tyran_object* o = evaluate(self, (const char*)self->script_buffer);

	return o;
}

static void send_script_object_resource(nimbus_script_module* self, tyran_object* script_object, nimbus_resource_id resource_id)
{
	nimbus_script_object_updated_send(&self->update.event_write_stream, resource_id, script_object);
}

static void parse_state(nimbus_script_module* self, tyran_object* state_script_object, nimbus_resource_id resource_id)
{
	nimbus_script_state_parser parser;

	nimbus_state state;
	nimbus_state_init(&state, self->memory, self->modules->component_definitions, self->modules->component_definitions_count);
	
	nimbus_script_state_parser_init(&parser, self->modules, self->mocha.default_runtime->symbol_table, self->resource_cache, &state, state_script_object, resource_id);
}

static void _on_resource_updated(void* _self, struct nimbus_event_read_stream* stream)
{
	nimbus_script_module* self = _self;
	nimbus_resource_updated* updated;

	nimbus_event_stream_read_type_pointer(stream, updated, nimbus_resource_updated);
	if (updated->resource_type_id == self->script_type_id || updated->resource_type_id == self->combine_script_type_id) {
		tyran_object* script_object = evaluate_stream(self, stream, updated->resource_id, updated->resource_type_id, updated->payload_size);
		if (updated->resource_type_id == self->combine_script_type_id) {
			parse_state(self, script_object, updated->resource_id);
		} else {
			send_script_object_resource(self, script_object, updated->resource_id);
		}
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
