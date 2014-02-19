#include "script_module.h"
#include "script_global.h"

/*
static void delete_callback(const tyran_runtime* runtime, struct tyran_object* object_to_be_deleted)
{
	nimbus_engine* _self = runtime->program_specific_context;
	nimbus_object_listener_on_delete(&_self->object_listener, object_to_be_deleted);
}
*/

static void _on_update(void* _self)
{
	nimbus_script_module* self = _self;
}

static void boot_script(tyran_mocha_api* mocha)
{
	tyran_mocha_api_new(mocha, 1024);
	mocha->default_runtime->program_specific_context = self;
	// mocha->default_runtime->delete_callback = delete_callback;

	nimbus_script_global_init(mocha);
}

static tyran_object* evaluate(nimbus_script_module* self, const char* data)
{
	tyran_value new_object = tyran_mocha_api_create_object(&self->mocha_api);
	tyran_object* object = tyran_value_mutable_object(&new_object);

	tyran_object_set_prototype(object, self->context);
	tyran_value temp_value;
	tyran_value_set_nil(temp_value);
	tyran_mocha_api_eval(self->mocha, &new_object, &temp_value, data);
	tyran_runtime_clear(self->runtime);


	return tyran_value_mutable_object(&new_object);
}

static tyran_object* evaluate_stream(nimbus_script_module* self, struct nimbus_event_read_stream* stream, nimbus_resource_id resource_id, nimbus_resource_type_id resource_type_id, int payload_size)
{
	TYRAN_ASSERT(payload_size <= self->script_buffer_size, "Buffer too small for script. payload:%d max:%d", payload_size, self->script_buffer_size);

	nimbus_event_stream_read_octets(stream, self->script_buffer, payload_size);
	self->script_buffer[payload_size] = 0;

	// TYRAN_LOG("*** EVALUATE *** %d octet_size:%d", resource_id, payload_size);
	// TYRAN_LOG("SCRIPT:'%s'", self->script_buffer);
	tyran_object* o = evaluate(self, (const char*)self->script_buffer);

	return o;
}

static void send_script_object_resource(nimbus_script_module* self, tyran_object* script_object, nimbus_resource_id resource_id)
{
	nimbus_script_object_updated_send(&self->update.event_write_stream, resource_id, script_object);
}



static void _on_resource_updated(void* _self, struct nimbus_event_read_stream* stream)
{
	nimbus_script_module* self = _self;
	nimbus_resource_updated* updated;

	nimbus_event_stream_read_type_pointer(stream, updated, nimbus_resource_updated);
	if (updated->resource_type_id == self->script_type_id || updated->resource_type_id == self->combine_script_type_id) {
		tyran_object* script_object = evaluate_stream(self, stream, updated->resource_id, updated->resource_type_id);
		if (updated->resource_type_id == self->combine_script_type_id) {
			parse_combine(self, script_object, resource_id);
		} else {
			send_script_object_resource(self, script_object, resource_id);
		}
	}
}

void nimbus_script_module_init(void* _self, struct tyran_memory* memory)
{
	nimbus_script_module* self = _self;
	// nimbus_object_listener_init(&self->object_listener, memory, &self->mocha_api, tyran_value_mutable_object(global), self->modules.event_definitions, self->modules.event_definitions_count);
	nimbus_update_init(&self->update, memory, _on_update, self, "script module");
	self->script_buffer_size = 16 * 1024;
	self->script_buffer = TYRAN_MEMORY_ALLOC(memory, self->script_buffer_size, "Script buffer");

	boot_script(&self->mocha_api);

	self->script_type_id = nimbus_resource_type_id_from_string("oes");
	self->combine_script_type_id = nimbus_resource_type_id_from_string("oec");
	nimbus_event_listener_listen(&self->update.event_listener, NIMBUS_EVENT_RESOURCE_UPDATED, _on_resource_updated);
}
