#ifndef nimbus_engine_h
#define nimbus_engine_h

#include <tyran_core/task/task.h>
#include <tyran_core/event/event_listener.h>
#include <tyran_core/event/event_stream.h>
#include "../../core/src/base/event/nimbus_event_distributor.h"

#include "../network/event_connection.h"

#include <tyran_engine/module/modules.h>

struct tyran_symbol_table;

struct nimbus_resource_handler;

typedef struct nimbus_engine {
	struct nimbus_resource_handler* resource_handler;
	nimbus_event_listener event_listener;
	nimbus_event_connection event_connection;
	nimbus_update update_object;

	nimbus_event_distributor event_distributor;
	nimbus_modules modules;

	nimbus_update** update_objects;
	int update_objects_count;

	struct tyran_symbol_table* symbol_table;
} nimbus_engine;

nimbus_engine* nimbus_engine_new(struct tyran_memory* memory, struct nimbus_task_queue* task_queue);
tyran_boolean nimbus_engine_should_render(nimbus_engine* engine);
int nimbus_engine_update(nimbus_engine* engine, struct nimbus_task_queue* queue);
void nimbus_engine_free(nimbus_engine* self);
void nimbus_engine_send_event(nimbus_engine* self, u8t event_id, void* data, int octet_count);

#endif
