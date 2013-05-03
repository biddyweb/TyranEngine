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

void nimbus_engine_work(void* _self, nimbus_task_queue* queue)
{
	nimbus_engine* self = (nimbus_engine*) _self;
	TYRAN_LOG("Engine WORK:%p",self);

}

void nimbus_engine_request_boot_resource(nimbus_engine* self)
{
	nimbus_resource_id boot_id = nimbus_resource_handler_add(self->resource_handler, "boot");

	nimbus_resource_load event;
	event.resource_id = boot_id;
	nimbus_event_stream_write_event(self->event_stream, NIMBUS_EVENT_RESOURCE_LOAD, event);
}


nimbus_engine* nimbus_engine_new(tyran_memory* memory)
{
	nimbus_engine* self = TYRAN_MEMORY_CALLOC_TYPE(memory, nimbus_engine);
	// self->listener = nimbus_event_listener_new(memory, self);
	//nimbus_event_listener_listen(self->listener, 1, );

	tyran_mocha_api_new(&self->mocha_api, 1024);

	tyran_value* global = tyran_runtime_context(self->mocha_api.default_runtime);
	tyran_mocha_api_add_function(&self->mocha_api, global, "load_library", nimbus_engine_load_library);

	self->task = nimbus_task_new(memory, nimbus_engine_work, self);
	self->resource_handler = nimbus_resource_handler_new(memory);
	self->event_listener = nimbus_event_listener_new(memory);

	nimbus_engine_request_boot_resource(self);

	return self;
}


tyran_boolean nimbus_engine_should_render(nimbus_engine* self)
{
	return TYRAN_FALSE;
}

void nimbus_engine_update(nimbus_engine* self, nimbus_task_queue* queue)
{
	nimbus_task_queue_add_task(queue, self->task);
}
