#include <tyran_core/update/update.h>
#include <tyranscript/tyran_log.h>

void nimbus_update_update(void* _self, struct nimbus_task_queue* queue)
{
	nimbus_update* self = _self;
	nimbus_event_read_stream* read_stream = &self->event_read_stream;
	nimbus_event_process(&self->event_listener, read_stream);

	TYRAN_ASSERT(self->update_function != 0, "null pointer as update_function");
	self->update_function(self->update_function_self);
}

void nimbus_update_init_ex(nimbus_update* self, struct tyran_memory* memory, nimbus_update_function func, void* update_self, int max_size, const char* name)
{
	TYRAN_ASSERT(func != 0, "Must give me a proper function");
	self->update_function = func;
	self->update_function_self = update_self;
	nimbus_event_write_stream_init(&self->event_write_stream, memory, max_size);
	self->name = name;
	nimbus_task_init(&self->task, nimbus_update_update, self, name);
	nimbus_event_stream_read_init(&self->event_read_stream, 0, 0);
	nimbus_event_listener_init(&self->event_listener, update_self);
}

void nimbus_update_init(nimbus_update* self, struct tyran_memory* memory, nimbus_update_function func, void* update_self, const char* name)
{
	const int max_size = 1024;
	nimbus_update_init_ex(self, memory, func, update_self, max_size, name);
}
