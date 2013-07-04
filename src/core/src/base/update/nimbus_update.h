#ifndef nimbus_update_h
#define nimbus_update_h

#include <tyranscript/tyran_types.h>

#include "../event/nimbus_event_stream.h"
#include "../event/nimbus_event_listener.h"
#include "../task/nimbus_task.h"

typedef void (*nimbus_update_function)(void*);

typedef struct nimbus_update {
	nimbus_event_write_stream event_write_stream;
	nimbus_event_read_stream event_read_stream;
	nimbus_event_listener event_listener;
	nimbus_task task;


	nimbus_update_function update_function;
	void* update_function_self;

	const char* name;
} nimbus_update;


void nimbus_update_init(nimbus_update* self, struct tyran_memory* memory, nimbus_update_function func, void* func_self, const char* name);
void nimbus_update_init_ex(nimbus_update* self, struct tyran_memory* memory, nimbus_update_function func, void* update_self, int max_size, const char* name);
#endif
