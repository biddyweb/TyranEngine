#include "object_loader.h"
#include "../event/resource_updated.h"
#include <tyranscript/tyran_mocha_api.h>

static void evaluate(nimbus_object_loader* self, const char* data, size_t len, tyran_value* return_value)
{
	tyran_mocha_api_eval(self->mocha, self->context, return_value, data, len);
}

static void add_object(nimbus_object_loader* self, tyran_value* value)
{
	
}

static void on_resource_updated(nimbus_object_loader* self, struct nimbus_event_read_stream* stream, nimbus_resource_id resource_id, int payload_size)
{
	TYRAN_ASSERT(payload_size <= self->script_buffer_size, "Buffer too small for script");
	
	nimbus_event_stream_read_octets(stream, self->script_buffer, payload_size);
	
	tyran_value return_value;
	evaluate(self, (const char*)self->script_buffer, payload_size, &return_value);
	add_object(self, &return_value);
}

static void _on_resource_updated(void* _self, struct nimbus_event_read_stream* stream)
{
	TYRAN_LOG("ObjectLoader::on_resource!!!!");
	nimbus_object_loader* self = _self;
	
	nimbus_resource_updated updated;
	
	nimbus_event_stream_read_type(stream, updated);
	
	on_resource_updated(self, stream, updated.resource_id, updated.payload_size);
}

static void _dummy_update(void* _self)
{
	TYRAN_LOG("Object Loader update");
}

void nimbus_object_loader_init(nimbus_object_loader* self, tyran_memory* memory, tyran_mocha_api* mocha, tyran_value* context)
{
	self->mocha = mocha;
	self->context = context;
	self->script_buffer_size = 1024;
	self->script_buffer = TYRAN_MEMORY_ALLOC(memory, self->script_buffer_size, "Script buffer");
	nimbus_update_init(&self->update, memory, _dummy_update, self);
	nimbus_event_listener_init(&self->update.event_listener, self);
	nimbus_event_listener_listen(&self->update.event_listener, NIMBUS_EVENT_RESOURCE_UPDATED, _on_resource_updated);
}