#include "object_listeners.h"

#include <tyranscript/tyran_object.h>
#include <tyranscript/tyran_symbol_table.h>
#include <tyranscript/tyran_runtime.h>
#include <tyranscript/tyran_function.h>
#include <tyranscript/tyran_property_iterator.h>

#include <tyranscript/debug/tyran_print_opcodes.h>

#include <tyran_engine/event/resource_updated.h>

#include <tyranscript/tyran_symbol_table.h>
#include "object_info.h"

static void info_add_function(nimbus_object_listener_info* self, tyran_value* function_context, const tyran_function* function)
{
	nimbus_object_listener_function* func_info = &self->functions[self->function_count++];
	tyran_value_copy(func_info->function_context, *function_context);
	func_info->function = function;
}


static void on_state_updated(nimbus_object_listener* self, tyran_object* o);

static void _on_resource_updated(void* _self, struct nimbus_event_read_stream* stream)
{
	nimbus_object_listener* self = _self;

	nimbus_resource_updated updated;

	nimbus_event_stream_read_type(stream, updated);
	if (updated.resource_type_id == self->state_type_id) {
		tyran_object* o;
		nimbus_event_stream_read_octets(stream, (u8t*)&o, sizeof(tyran_object*));
		on_state_updated(self, o);
	}
}

static nimbus_object_listener_info* find_info_from_symbol(nimbus_object_listener* self, tyran_symbol symbol)
{
	for (int i=0; i<self->info_count; ++i) {
		nimbus_object_listener_info* info = &self->infos[i];
		if (info->symbol.hash == symbol.hash) {
			return info;
		}
	}
	return 0;
}

static nimbus_object_listener_info* info_from_symbol(nimbus_object_listener* self, tyran_symbol symbol)
{
	nimbus_object_listener_info* info = find_info_from_symbol(self, symbol);
	if (!info) {
		info = &self->infos[self->info_count++];
		info->max_function_count = 32;
		info->symbol = symbol;
		info->function_count = 0;
	}
	return info;
}


static void call_event(nimbus_object_listener* self, tyran_symbol symbol)
{
	nimbus_object_listener_info* info = find_info_from_symbol(self, symbol);
	if (!info) {
		return;
	}


	if (info->function_count == 0) {
		TYRAN_LOG("No function count for that symbol");
		return;
	}


	tyran_value return_value;
	for (int i=0; i<info->function_count; ++i) {
		nimbus_object_listener_function* func_info = &info->functions[i];

#if defined OBJECT_LISTENERS_DEBUG
		TYRAN_LOG("UPDATE function:%p, function_context:%p", func_info->function, &func_info->function_context);
		tyran_print_value("context", &func_info->function_context, 1, self->symbol_table);
		tyran_print_opcodes(func_info->function->data.opcodes, 0, func_info->function->constants);
#endif
		tyran_runtime_clear(self->runtime);
		tyran_runtime_push_call_ex(self->runtime, func_info->function, &func_info->function_context);
		tyran_runtime_execute(self->runtime, &return_value, 0);
	}
}

static void _dummy_update_2(void* _self2)
{
	nimbus_object_listener* c = _self2;

	call_event(c, c->frame_symbol);
}

static void nimbus_object_layers_add_layer(nimbus_object_listener* self, const char* name, struct tyran_memory* memory)
{
	nimbus_object_layer* layer = &self->layers[self->layers_count++];
	layer->name = tyran_strdup(memory, name);
}


void nimbus_object_listener_init(nimbus_object_listener* self, tyran_memory* memory, tyran_runtime* runtime)
{
	self->runtime = runtime;
	self->symbol_table = runtime->symbol_table;
	nimbus_update_init(&self->update, memory, _dummy_update_2, self, "script listener");
	nimbus_event_listener_init(&self->update.event_listener, self);
	nimbus_event_listener_listen(&self->update.event_listener, NIMBUS_EVENT_RESOURCE_UPDATED, _on_resource_updated);
	self->state_type_id = nimbus_resource_type_id_from_string("state");
	tyran_symbol_table_add(runtime->symbol_table, &self->type_symbol, "type");

	tyran_symbol_table_add(self->symbol_table, &self->frame_symbol, "Frame");

	self->info_max_count = 64;
	self->info_count = 0;
	self->memory = memory;

	self->max_layers_count = 8;
	self->layers_count = 0;
	self->layers = TYRAN_MEMORY_CALLOC_TYPE_COUNT(memory, nimbus_object_layer, self->max_layers_count);

	nimbus_object_layers_add_layer(self, "render", memory);

	/*
		self->infos = TYRAN_MEMORY_CALLOC_TYPE_COUNT(memory, nimbus_object_listener_info, self->info_max_count);
		for (int i=0; i<self->info_max_count; ++i) {
			nimbus_object_listener_info* info = &self->infos[i];
			info->max_function_count = 64;
			info->functions = TYRAN_MEMORY_CALLOC_TYPE_COUNT(memory, nimbus_object_listener_function, info->max_function_count);
		}
	*/
}


static void add_listening_function(nimbus_object_listener* self, tyran_value* function_context, tyran_value* function, const char* event_name)
{
	tyran_symbol symbol;

	tyran_symbol_table_add(self->symbol_table, &symbol, event_name);
	nimbus_object_listener_info* info = info_from_symbol(self, symbol);
	info_add_function(info, function_context, tyran_value_function(function));
}


static void scan_for_listening_functions_on_object(nimbus_object_listener* self, tyran_object* o, tyran_object* combine)
{
	tyran_property_iterator it;

	tyran_property_iterator_init(&it, o);

	tyran_symbol symbol;
	tyran_value* value;

	while (tyran_property_iterator_next(&it, &symbol, &value)) {
		const char* debug_key_string = tyran_symbol_table_lookup(self->symbol_table, &symbol);
		if (tyran_value_is_object(value)) {
			if (tyran_value_is_function(value)) {
				if (debug_key_string[0] == 'o' && debug_key_string[1] == 'n') {
					tyran_value object_value;
					tyran_value_set_object(object_value, o);
					add_listening_function(self, &object_value, value, &debug_key_string[2]);
				}
			}
		}
	}

	tyran_property_iterator_free(&it);
}

static nimbus_resource_id resource_id_for_layer(const char* layer_name, tyran_symbol_table* symbol_table, tyran_symbol symbol)
{
	const char* type_name_string = tyran_symbol_table_lookup(symbol_table, &symbol);

	const int temp_buf_size = 128;
	char temp[temp_buf_size];
	tyran_strncpy(temp, temp_buf_size, layer_name, tyran_strlen(layer_name));
	tyran_strncat(temp, "/", temp_buf_size);
	tyran_strncat(temp, type_name_string, temp_buf_size);

	TYRAN_LOG("Layer-specific name:'%s'", temp);
	nimbus_resource_id layer_specific_resource_id = nimbus_resource_id_from_string(temp);

	return layer_specific_resource_id;
}


static void decorate_object(tyran_object* o, struct tyran_memory* memory, tyran_symbol symbol)
{
	TYRAN_LOG("decorate");
	nimbus_object_info* info = TYRAN_MEMORY_CALLOC_TYPE(memory, nimbus_object_info);
	info->symbol = symbol;
	info->instance_id = 0; // self->instance_id++;
	info->layers_count = 0;

	void* current = tyran_object_program_specific(o);
	TYRAN_ASSERT(current == 0, "Current must be null");
	tyran_object_set_program_specific(o, info);
}

static void trigger_spawn_in_layers(nimbus_object_listener* self, tyran_symbol type_name)
{
	for (int i=0; i<self->layers_count; ++i) {
		nimbus_object_layer* layer = &self->layers[i];
		resource_id_for_layer(layer->name, self->symbol_table, type_name);
	}
}

static void handle_type_object(nimbus_object_listener* self, tyran_object* o, tyran_symbol type_name, const char* type_name_string)
{
	TYRAN_LOG("Found type: '%s'", type_name_string);
	decorate_object(o, self->memory, type_name);
	trigger_spawn_in_layers(self, type_name);
}

static void check_for_type_on_component(nimbus_object_listener* self, tyran_object* component)
{
	tyran_value found_value;
	tyran_object_lookup_prototype(&found_value, component, &self->type_symbol);
	if (tyran_value_is_symbol(&found_value)) {
		tyran_symbol type_value = tyran_value_symbol(&found_value);
		const char* type_value_string = tyran_symbol_table_lookup(self->symbol_table, &type_value);
		handle_type_object(self, component, type_value, type_value_string);
	}
}

static void scan_component(nimbus_object_listener* self, tyran_object* component, tyran_object* combine)
{
	scan_for_listening_functions_on_object(self, component, combine);
	check_for_type_on_component(self, component);
}


static void scan_combine(nimbus_object_listener* self, tyran_object* combine)
{
	tyran_property_iterator it;

	tyran_property_iterator_init_shallow(&it, combine);

	tyran_symbol symbol;
	tyran_value* value;

	while (tyran_property_iterator_next(&it, &symbol, &value)) {
		if (tyran_value_is_object(value)) {
			if (!tyran_value_is_function(value)) {
				const char* debug_key_string = tyran_symbol_table_lookup(self->symbol_table, &symbol);
				TYRAN_LOG("Component: '%s'", debug_key_string);
				scan_component(self, tyran_value_object(value), combine);
			}
		}
	}

	tyran_property_iterator_free(&it);
}


static void on_state_updated(nimbus_object_listener* self, tyran_object* o)
{
	scan_combine(self, o);
}
