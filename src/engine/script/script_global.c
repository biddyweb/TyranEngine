#include "script_global.h"

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
	if (tyran_value_is_string(v)) {
		print_value(runtime, v);
	} else {
		tyran_print_value("log", v, 1, runtime->symbol_table);
	}
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

TYRAN_RUNTIME_CALL_FUNC(script_sin)
{
	float value = nimbus_math_sin(tyran_value_number(&arguments[0]));
	tyran_value_set_number(*return_value, value);
	return 0;
}

TYRAN_RUNTIME_CALL_FUNC(script_spawn)
{
	nimbus_engine* _self = runtime->program_specific_context;

	tyran_object* spawned_object = nimbus_object_listener_spawn(&_self->object_listener, tyran_value_object(arguments));
	tyran_value_replace_object(*return_value, spawned_object);
	return 0;
}

TYRAN_RUNTIME_CALL_FUNC(script_unspawn)
{
	nimbus_engine* _self = runtime->program_specific_context;
	nimbus_combine_instance_id combine_instance_id = (nimbus_combine_instance_id) tyran_value_number(arguments);
	nimbus_object_listener_unspawn(&_self->object_listener, combine_instance_id);
	return 0;
}

void nimbus_script_global_init()
{
	tyran_value* global = tyran_runtime_context(mocha->default_runtime);
	tyran_mocha_api_add_function(mocha, global, "load_library", nimbus_engine_load_library);
	tyran_mocha_api_add_function(mocha, global, "loadState", load_state);
	tyran_mocha_api_add_function(mocha, global, "log", log_output);
	tyran_mocha_api_add_function(mocha, global, "random", script_random);
	tyran_mocha_api_add_function(mocha, global, "abs", script_abs);
	tyran_mocha_api_add_function(mocha, global, "atan2", script_atan2);
	tyran_mocha_api_add_function(mocha, global, "sin", script_sin);
	tyran_mocha_api_add_function(mocha, global, "spawn", script_spawn);
	tyran_mocha_api_add_function(mocha, global, "unspawn", script_unspawn);
}
