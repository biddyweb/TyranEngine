#include "nimbus_engine.h"
#include "../../core/src/base/task/nimbus_task_queue.h"
#include "../resource/resource_id.h"
#include "../resource/resource_handler.h"

#include "../event/resource_load.h"
#include "../../core/src/base/event/nimbus_event_stream.h"
#include "../../core/src/base/event/nimbus_event_listener.h"


TYRAN_RUNTIME_CALL_FUNC(nimbus_engine_load_library)
{
	return 0;
}

int nimbus_engine_update(nimbus_engine* self, nimbus_task_queue* queue)
{
//	nimbus_task_queue_add()
	TYRAN_LOG("Engine::Update");
	self->frame_counter++;
	if (self->frame_counter > 10) {
		return 1;
	}
	return 0;
}


static void boot_resource(nimbus_engine* self)
{
	nimbus_resource_id boot_id = nimbus_resource_handler_add(self->resource_handler, "boot");

	nimbus_resource_load event;
	event.resource_id = boot_id;
	nimbus_event_stream_write_event(&self->event_stream, NIMBUS_EVENT_RESOURCE_LOAD, event);
}


void start_event_connection(nimbus_engine* self, tyran_memory* memory, const char* host, int port)
{
	nimbus_event_connection_init(&self->event_connection, memory, host, port);
}

nimbus_engine* nimbus_engine_new(tyran_memory* memory)
{
	nimbus_engine* self = TYRAN_MEMORY_CALLOC_TYPE(memory, nimbus_engine);
	self->frame_counter = 0;
	// self->listener = nimbus_event_listener_new(memory, self);
	//nimbus_event_listener_listen(self->listener, 1, );

	tyran_mocha_api_new(&self->mocha_api, 1024);

	tyran_value* global = tyran_runtime_context(self->mocha_api.default_runtime);
	tyran_mocha_api_add_function(&self->mocha_api, global, "load_library", nimbus_engine_load_library);

	nimbus_event_write_stream_init(&self->event_stream, memory, 1024);

	self->resource_handler = nimbus_resource_handler_new(memory);
	nimbus_event_listener_init(&self->event_listener);

	start_event_connection(self, memory, "198.74.60.114", 32000);

	boot_resource(self);

	return self;
}

void nimbus_engine_free(nimbus_engine* self)
{
	TYRAN_LOG("Freeing up engine...");
	nimbus_event_write_stream_free(&self->event_stream);
}


tyran_boolean nimbus_engine_should_render(nimbus_engine* self)
{
	return TYRAN_FALSE;
}

