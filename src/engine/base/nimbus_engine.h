#ifndef nimbus_engine_h
#define nimbus_engine_h

#include "../../core/src/base/task/nimbus_task.h"
#include <tyranscript/tyran_mocha_api.h>

struct nimbus_resource_handler;
struct nimbus_event_write_stream;
struct nimbus_event_listener;

typedef struct nimbus_engine {
	nimbus_task* task;
	tyran_mocha_api mocha_api;
	struct nimbus_resource_handler* resource_handler;
	struct nimbus_event_write_stream* event_stream;
	struct nimbus_event_listener* event_listener;
} nimbus_engine;


nimbus_engine* nimbus_engine_new(tyran_memory* memory);
tyran_boolean nimbus_engine_should_render(nimbus_engine* engine);
void nimbus_engine_update(nimbus_engine* engine, struct nimbus_task_queue* queue);

#endif