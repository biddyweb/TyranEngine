#include "nimbus_engine.h"
#include "../../core/src/base/task/nimbus_task_queue.h"
#include <tyran_engine/resource/id.h>
#include "../resource/resource_handler.h"

#include <tyran_engine/event/resource_load.h>
#include <tyran_engine/event/touch_changed.h>
#include <tyran_engine/event/key_changed.h>


#include "../event/resource_load_state.h"

#include <tyran_core/event/event_stream.h>
#include <tyran_core/event/event_listener.h>
#include "../resource/resource_handler.h"

#include <tyran_engine/module/nimbus_module.h>
#include <tyran_engine/module/register_modules.h>

#include <tyran_engine/math/nimbus_math.h>

#if defined TORNADO_OS_NACL
#include "../../core/src/platform/nacl/nacl_loader.h"
#include "../../core/src/platform/nacl/nacl_input.h"
#include "../../core/src/platform/nacl/nacl_connection.h"
#include "../../core/src/platform/nacl/nacl_gamepad.h"
#endif

static void fire_load_state(nimbus_engine* self, nimbus_resource_id id)
{
	nimbus_resource_load_state_send(&self->update_object.event_write_stream, id);
}

static void fire_load_resource(nimbus_engine* self, nimbus_resource_id id, nimbus_resource_type_id resource_type_id)
{
	nimbus_resource_load_send(&self->update_object.event_write_stream, id, resource_type_id);
}


static void on_load_state(nimbus_engine* self, const char* state_name)
{
	nimbus_resource_id id = nimbus_resource_id_from_string(state_name);
	fire_load_state(self, id);
}

TYRAN_RUNTIME_CALL_FUNC(nimbus_engine_load_library)
{
	return 0;
}

TYRAN_RUNTIME_CALL_FUNC(load_state)
{
	const struct tyran_string* state_name = tyran_value_string(&arguments[0]);
	char state_name_buf[1024];
	tyran_string_to_c_str(state_name_buf, 1024, state_name);


	TYRAN_LOG("LoadState: '%s'", state_name_buf);
	nimbus_engine* _self = runtime->program_specific_context;

	on_load_state(_self, state_name_buf);
	return 0;
}

static void print_value(tyran_runtime* runtime, tyran_value* v)
{
	const int buf_len = 512;
	char buf[buf_len];
	tyran_value_to_c_string(runtime->symbol_table, v, buf, buf_len, 0);
	TYRAN_OUTPUT(buf);
}

TYRAN_RUNTIME_CALL_FUNC(log_output)
{
	struct tyran_value* v = &arguments[0];
	print_value(runtime, v);
	return 0;
}

TYRAN_RUNTIME_CALL_FUNC(script_random)
{
	float random_value = (rand() % 512) / 511.0f;
	tyran_value_set_number(*return_value, random_value);
	return 0;
}


TYRAN_RUNTIME_CALL_FUNC(script_abs)
{
	float value = nimbus_math_fabs(tyran_value_number(arguments));
	tyran_value_set_number(*return_value, value);
	return 0;
}

TYRAN_RUNTIME_CALL_FUNC(script_atan2)
{
	float value = nimbus_math_atan2(tyran_value_number(&arguments[0]), tyran_value_number(&arguments[1]));
	tyran_value_set_number(*return_value, value);
	return 0;
}


TYRAN_RUNTIME_CALL_FUNC(script_spawn)
{
	nimbus_engine* _self = runtime->program_specific_context;

	tyran_object* spawned_object = nimbus_object_listener_spawn(&_self->object_listener, tyran_value_object(arguments));
	tyran_value_replace_object(*return_value, spawned_object);
	tyran_object_release(spawned_object);
	TYRAN_ASSERT(tyran_object_program_specific(spawned_object) != 0, "Spawned must have program specific");
	return 0;
}

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

	self->frame_counter++;
	if (self->frame_counter > 9999) {
		return 1;
	}
	return 0;
}

static void _dummy_update(void* _self)
{
	TYRAN_LOG("Engine::dummy_Update");
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
		TYRAN_LOG("Creating '%s'", module->name);
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

}

static void delete_callback(const tyran_runtime* runtime, struct tyran_object* object_to_be_deleted)
{
	nimbus_engine* _self = runtime->program_specific_context;
	nimbus_object_listener_on_delete(&_self->object_listener, object_to_be_deleted);
}


nimbus_engine* nimbus_engine_new(tyran_memory* memory, struct nimbus_task_queue* task_queue)
{
	nimbus_engine* self = TYRAN_MEMORY_CALLOC_TYPE(memory, nimbus_engine);
	self->frame_counter = 0;
	// self->listener = nimbus_event_listener_new(memory, self);
	//nimbus_event_listener_listen(self->listener, 1, );

	tyran_mocha_api_new(&self->mocha_api, 1024);
	self->mocha_api.default_runtime->program_specific_context = self;
	self->mocha_api.default_runtime->delete_callback = delete_callback;

	nimbus_event_distributor_init(&self->event_distributor, self->mocha_api.default_runtime->symbol_table, memory);


	tyran_value* global = tyran_runtime_context(self->mocha_api.default_runtime);
	tyran_mocha_api_add_function(&self->mocha_api, global, "load_library", nimbus_engine_load_library);
	tyran_mocha_api_add_function(&self->mocha_api, global, "loadState", load_state);
	tyran_mocha_api_add_function(&self->mocha_api, global, "log", log_output);
	tyran_mocha_api_add_function(&self->mocha_api, global, "random", script_random);
	tyran_mocha_api_add_function(&self->mocha_api, global, "abs", script_abs);
	tyran_mocha_api_add_function(&self->mocha_api, global, "atan2", script_atan2);
	tyran_mocha_api_add_function(&self->mocha_api, global, "spawn", script_spawn);

	self->resource_handler = nimbus_resource_handler_new(memory);
	nimbus_event_listener_init(&self->event_listener, self);

	self->update_objects = TYRAN_MEMORY_CALLOC_TYPE_COUNT(memory, nimbus_update*, 256);
	self->update_objects_count = 0;

	nimbus_update_init(&self->update_object, memory, _dummy_update, 0, "engine");
	nimbus_engine_add_update_object(self, &self->update_object);

	nimbus_modules_init(&self->modules, memory, self->mocha_api.default_runtime->symbol_table);
	nimbus_register_modules(&self->modules);
	add_internal_modules(&self->modules);

	nimbus_object_listener_init(&self->object_listener, memory, &self->mocha_api, tyran_value_mutable_object(global), self->modules.event_definitions, self->modules.event_definitions_count);
	nimbus_engine_add_update_object(self, &self->object_listener.update);



	create_modules(self, memory);

#if defined TORNADO_OS_IOS

	start_event_connection(self, memory, "spelmotor.com", 32000, task_queue);

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

