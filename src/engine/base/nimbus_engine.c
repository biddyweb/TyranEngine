#include "nimbus_engine.h"

#include "../../core/src/base/task/nimbus_task_queue.h"
#include <tyran_engine/resource/id.h>
#include "../resource/resource_handler.h"
#include <tyran_engine/event/resource_load.h>
#include <tyran_engine/event/touch_changed.h>
#include <tyran_engine/event/key_changed.h>

#include <tyran_core/event/event_stream.h>
#include <tyran_core/event/event_listener.h>
#include "../resource/resource_handler.h"
#include <tyran_engine/module/nimbus_module.h>
#include <tyran_engine/module/register_modules.h>
#include <tyran_engine/math/nimbus_math.h>
#include <tyranscript/tyran_symbol_table.h>

#include "../script/script_module.h"

#if defined TORNADO_OS_NACL
#include "../../core/src/platform/nacl/nacl_loader.h"
#include "../../core/src/platform/nacl/nacl_input.h"
#include "../../core/src/platform/nacl/nacl_connection.h"
#include "../../core/src/platform/nacl/nacl_gamepad.h"
#endif

nimbus_modules* g_modules;

void schedule_update_tasks(nimbus_engine* self, nimbus_task_queue* queue)
{
	for (int i=1; i<self->update_objects_count; ++i) {
		nimbus_task* task = &self->update_objects[i]->task;
		// TYRAN_LOG("adding task:%d (%s)", i, self->update_objects[i]->name);
		nimbus_task_queue_add_task(queue, task);
	}
}

void distribute_events(nimbus_engine* self)
{
	nimbus_event_distributor_distribute_events(&self->event_distributor, self->update_objects, self->update_objects_count);
}

int nimbus_engine_update(nimbus_engine* self, nimbus_task_queue* queue)
{
	distribute_events(self);
	schedule_update_tasks(self, queue);

	return 0;
}

static void _dummy_update(void* _self)
{
	TYRAN_LOG("Engine::dummy_Update");
}



static void fire_load_resource(nimbus_engine* self, nimbus_resource_id id, nimbus_resource_type_id resource_type_id)
{
	nimbus_resource_load_send(&self->update_object.event_write_stream, id, resource_type_id);
}


static void boot_resource(nimbus_engine* self)
{
	nimbus_resource_id boot_id = nimbus_resource_handler_add(self->resource_handler, "boot");

	nimbus_resource_type_id resource_type_id = nimbus_resource_type_id_from_string("script");

	fire_load_resource(self, boot_id, resource_type_id);
}

void nimbus_engine_send_event(nimbus_engine* self, u8t event_id, void* data, int octet_count)
{
	nimbus_event_write_stream* stream = &self->update_object.event_write_stream;
	nimbus_event_stream_write_event_header(stream, event_id);
	nimbus_event_stream_write_align(stream);
	nimbus_event_stream_write_octets(stream, data, octet_count);
	nimbus_event_stream_write_event_end(stream);
}

void nimbus_engine_add_update_object(nimbus_engine* self, nimbus_update* o)
{
	int index = self->update_objects_count++;
	o->task.group = 1;
	self->update_objects[index] = o;
}

#if defined TORNADO_OS_IOS

void start_event_connection(nimbus_engine* self, tyran_memory* memory, const char* host, int port, struct nimbus_task_queue* task_queue)
{
	nimbus_event_connection_init(&self->event_connection, memory, host, port);
	nimbus_engine_add_update_object(self, &self->event_connection.update_object);
	nimbus_task_queue_add_task(task_queue, &self->event_connection.receive_task);
}

#endif

static void create_modules(nimbus_engine* self, tyran_memory* memory)
{
	nimbus_modules* modules = &self->modules;
	for (int i=0; i < modules->modules_count; ++i) {
		nimbus_module* module = &modules->modules[i];
		void* instance = nimbus_module_create(module, memory);
		nimbus_update* instance_update = nimbus_module_get_update(module, instance);
		nimbus_engine_add_update_object(self, instance_update);
		if (module->affinity != -1) {
			instance_update->task.group = 0;
			instance_update->task.affinity = module->affinity;
		}
	}
}

static void add_internal_modules(nimbus_modules* modules)
{
	nimbus_event_definition* touch_began = nimbus_modules_add_event(modules, "touch_began", NIMBUS_EVENT_TOUCH_BEGAN_ID, 0);
	nimbus_event_definition_add_property(touch_began, "position", NIMBUS_EVENT_DEFINITION_VECTOR2);
	nimbus_event_definition_add_property(touch_began, "finger_id", NIMBUS_EVENT_DEFINITION_INTEGER);
	touch_began->is_module_to_script = TYRAN_TRUE;
	touch_began->has_index = TYRAN_FALSE;

	nimbus_event_definition* touch_moved = nimbus_modules_add_event(modules, "touch_moved", NIMBUS_EVENT_TOUCH_MOVED_ID, 0);
	nimbus_event_definition_add_property(touch_moved, "position", NIMBUS_EVENT_DEFINITION_VECTOR2);
	nimbus_event_definition_add_property(touch_moved, "finger_id", NIMBUS_EVENT_DEFINITION_INTEGER);
	touch_moved->is_module_to_script = TYRAN_TRUE;
	touch_moved->has_index = TYRAN_FALSE;

	nimbus_event_definition* touch_ended = nimbus_modules_add_event(modules, "touch_ended", NIMBUS_EVENT_TOUCH_ENDED_ID, 0);
	nimbus_event_definition_add_property(touch_ended, "position", NIMBUS_EVENT_DEFINITION_VECTOR2);
	nimbus_event_definition_add_property(touch_ended, "finger_id", NIMBUS_EVENT_DEFINITION_INTEGER);
	touch_ended->is_module_to_script = TYRAN_TRUE;
	touch_ended->has_index = TYRAN_FALSE;

	nimbus_event_definition* touch_zoom = nimbus_modules_add_event(modules, "touch_zoom", NIMBUS_EVENT_TOUCH_ZOOM_ID, 0);
	nimbus_event_definition_add_property(touch_zoom, "position", NIMBUS_EVENT_DEFINITION_VECTOR2);
	touch_zoom->is_module_to_script = TYRAN_TRUE;
	touch_zoom->has_index = TYRAN_FALSE;

	nimbus_event_definition* touch_stationary = nimbus_modules_add_event(modules, "touch_stationary", NIMBUS_EVENT_TOUCH_STATIONARY_ID, 0);
	nimbus_event_definition_add_property(touch_stationary, "position", NIMBUS_EVENT_DEFINITION_VECTOR2);
	nimbus_event_definition_add_property(touch_stationary, "finger_id", NIMBUS_EVENT_DEFINITION_INTEGER);
	touch_stationary->is_module_to_script = TYRAN_TRUE;
	touch_stationary->has_index = TYRAN_FALSE;

	nimbus_event_definition* key_changed = nimbus_modules_add_event(modules, "key_changed", NIMBUS_EVENT_KEY_CHANGED_ID, 0);
	nimbus_event_definition_add_property(key_changed, "virtual_key", NIMBUS_EVENT_DEFINITION_INTEGER);
	nimbus_event_definition_add_property(key_changed, "key_is_down", NIMBUS_EVENT_DEFINITION_BOOLEAN);
	key_changed->is_module_to_script = TYRAN_TRUE;
	key_changed->has_index = TYRAN_FALSE;

#if defined TORNADO_OS_NACL
	nimbus_modules_add_affinity(modules, "nacl_loader", sizeof(nimbus_nacl_loader), nimbus_nacl_loader_init, offsetof(nimbus_nacl_loader, update_object), 0);
	nimbus_modules_add_affinity(modules, "nacl_gamepad", sizeof(nimbus_nacl_gamepad), nimbus_nacl_gamepad_init, offsetof(nimbus_nacl_gamepad, update), 0);
	nimbus_modules_add_affinity(modules, "nacl_input", sizeof(nimbus_nacl_input), nimbus_nacl_input_init, offsetof(nimbus_nacl_input, update), 0);
	nimbus_modules_add_affinity(modules, "nacl_connection", sizeof(nimbus_nacl_connection), nimbus_nacl_connection_init, offsetof(nimbus_nacl_connection, update), 0);
#endif

	nimbus_modules_add(modules, "script", sizeof(nimbus_script_module), nimbus_script_module_init, offsetof(nimbus_script_module, update));
}

nimbus_engine* nimbus_engine_new(tyran_memory* memory, struct nimbus_task_queue* task_queue)
{
	nimbus_engine* self = TYRAN_MEMORY_CALLOC_TYPE(memory, nimbus_engine);
	
	self->symbol_table = tyran_symbol_table_new(memory);

	nimbus_event_distributor_init(&self->event_distributor, self->symbol_table, memory);

	self->resource_handler = nimbus_resource_handler_new(memory);
	nimbus_event_listener_init(&self->event_listener, self);

	self->update_objects = TYRAN_MEMORY_CALLOC_TYPE_COUNT(memory, nimbus_update*, 256);
	self->update_objects_count = 0;

	nimbus_update_init(&self->update_object, memory, _dummy_update, 0, "engine");
	nimbus_engine_add_update_object(self, &self->update_object);

	g_modules = &self->modules;
	nimbus_modules_init(&self->modules, memory, self->symbol_table);
	nimbus_register_modules(&self->modules);
	add_internal_modules(&self->modules);

	create_modules(self, memory);

#if defined TORNADO_OS_IOS
#if 0
	start_event_connection(self, memory, "spelmotor.com", 32000, task_queue);
#else
	start_event_connection(self, memory, "127.0.0.1", 32000, task_queue);
#endif
#endif

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
