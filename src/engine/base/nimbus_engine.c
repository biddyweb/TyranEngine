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


void schedule_update_tasks(nimbus_engine* self, nimbus_task_queue* queue)
{
	for (int i=1; i<self->update_objects_count; ++i) {
		nimbus_task* task = &self->update_objects[i]->task;
		nimbus_task_queue_add_task(queue, task);
	}
}

void distribute_events(nimbus_engine* self)
{
	nimbus_event_distributor_distribute_events(&self->event_distributor, self->update_objects, self->update_objects_count);
}

int nimbus_engine_update(nimbus_engine* self, nimbus_task_queue* queue)
{
//	nimbus_task_queue_add()
	TYRAN_LOG("Engine::Update");

	distribute_events(self);
	schedule_update_tasks(self, queue);

	self->frame_counter++;
	if (self->frame_counter > 0) {
		return 1;
	}
	return 0;
}

static void _dummy_update(void* _self)
{
	
}

static void boot_resource(nimbus_engine* self)
{
	nimbus_resource_id boot_id = nimbus_resource_handler_add(self->resource_handler, "boot");
	TYRAN_LOG("boot resource id:%d", boot_id);

	nimbus_resource_load event;
	event.resource_id = boot_id;
	nimbus_event_stream_write_event(&self->update_object.event_write_stream, NIMBUS_EVENT_RESOURCE_LOAD, event);
}


void nimbus_engine_add_update_object(nimbus_engine* self, nimbus_update* o)
{
	int index = self->update_objects_count++;
	self->update_objects[index] = o;
}

void start_event_connection(nimbus_engine* self, tyran_memory* memory, const char* host, int port)
{
	nimbus_event_connection_init(&self->event_connection, memory, host, port);
	nimbus_engine_add_update_object(self, &self->event_connection.update_object);
}

nimbus_engine* nimbus_engine_new(tyran_memory* memory)
{
	nimbus_engine* self = TYRAN_MEMORY_CALLOC_TYPE(memory, nimbus_engine);
	self->frame_counter = 0;
	// self->listener = nimbus_event_listener_new(memory, self);
	//nimbus_event_listener_listen(self->listener, 1, );

	tyran_mocha_api_new(&self->mocha_api, 1024);
	
	nimbus_event_distributor_init(&self->event_distributor, memory);
	

	tyran_value* global = tyran_runtime_context(self->mocha_api.default_runtime);
	tyran_mocha_api_add_function(&self->mocha_api, global, "load_library", nimbus_engine_load_library);

	self->resource_handler = nimbus_resource_handler_new(memory);
	nimbus_event_listener_init(&self->event_listener, self);
	
	self->update_objects = TYRAN_MEMORY_CALLOC_TYPE_COUNT(memory, nimbus_update*, 256);
	self->update_objects_count = 0;
	
	nimbus_update_init(&self->update_object, memory, _dummy_update, 0);
	
	nimbus_engine_add_update_object(self, &self->update_object);
	start_event_connection(self, memory, "198.74.60.114", 32000);

	boot_resource(self);

	return self;
}


void nimbus_engine_free(nimbus_engine* self)
{
	TYRAN_LOG("Freeing up engine...");
}


tyran_boolean nimbus_engine_should_render(nimbus_engine* self)
{
	return TYRAN_FALSE;
}

