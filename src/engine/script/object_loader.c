#include "object_loader.h"
#include "../event/resource_updated.h"
#include <tyranscript/tyran_mocha_api.h>

static void evaluate(nimbus_object_loader* self, const char* data, tyran_value* return_value)
{
	tyran_value new_object = tyran_mocha_api_create_object(self->mocha);

	tyran_value_object_set_prototype(&new_object, self->context);
	tyran_value temp_value;
	tyran_value_set_nil(temp_value);
	tyran_mocha_api_eval(self->mocha, &new_object, &temp_value, data);
	tyran_value_copy(*return_value, new_object);
}

static void send_resource_update(nimbus_event_write_stream* out_event_stream, nimbus_resource_id resource_id, nimbus_resource_type_id type_id, tyran_object* object)
{
	nimbus_resource_updated updated;
	updated.resource_id = resource_id;
	updated.resource_type_id = type_id;
	updated.payload_size = sizeof(tyran_object*);

	nimbus_event_stream_write_event_header(out_event_stream, NIMBUS_EVENT_RESOURCE_UPDATED);
	nimbus_event_stream_write_octets(out_event_stream, &object, sizeof(tyran_object*));
}

static void add_object(nimbus_object_loader* self, nimbus_resource_id resource_id, tyran_value* value)
{
	TYRAN_LOG("add resolved object(%d)", resource_id);
	nimbus_dependency_resolver_object_loaded(&self->dependency_resolver, value, resource_id);
	if (nimbus_dependency_resolver_done(&self->dependency_resolver)) {
		TYRAN_LOG("******** We have loaded!!!!! *********");
		// send_resource_update
	}
}

static void on_resource_updated(nimbus_object_loader* self, struct nimbus_event_read_stream* stream, nimbus_resource_id resource_id, int payload_size)
{
	TYRAN_ASSERT(payload_size <= self->script_buffer_size, "Buffer too small for script. payload:%d max:%d", payload_size, self->script_buffer_size);

	nimbus_event_stream_read_octets(stream, self->script_buffer, payload_size);
	self->script_buffer[payload_size] = 0;

	TYRAN_LOG("*** EVALUATE *** %d octet_size:%d", resource_id, payload_size);
	TYRAN_LOG("SCRIPT:'%s'", self->script_buffer);
	tyran_value return_value;
	evaluate(self, (const char*)self->script_buffer, &return_value);
	add_object(self, resource_id, &return_value);
}

static void _on_resource_updated(void* _self, struct nimbus_event_read_stream* stream)
{
	nimbus_object_loader* self = _self;

	nimbus_resource_updated updated;

	nimbus_event_stream_read_type(stream, updated);

	on_resource_updated(self, stream, updated.resource_id, updated.payload_size);
}

static void _dummy_update(void* _self)
{
}

void nimbus_object_loader_init(nimbus_object_loader* self, tyran_memory* memory, tyran_mocha_api* mocha, tyran_value* context)
{
	self->mocha = mocha;
	self->context = context;
	self->script_buffer_size = 16 * 1024;
	self->script_buffer = TYRAN_MEMORY_ALLOC(memory, self->script_buffer_size, "Script buffer");
	nimbus_update_init(&self->update, memory, _dummy_update, self, "loader");
	nimbus_event_listener_init(&self->update.event_listener, self);
	nimbus_event_listener_listen(&self->update.event_listener, NIMBUS_EVENT_RESOURCE_UPDATED, _on_resource_updated);

	nimbus_dependency_resolver_init(&self->dependency_resolver, memory, mocha->default_runtime->symbol_table, &self->update.event_write_stream);
}
