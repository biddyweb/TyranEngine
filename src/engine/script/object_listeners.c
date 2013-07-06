#include "object_listeners.h"

#include <tyranscript/tyran_object.h>
#include <tyranscript/tyran_symbol_table.h>
#include <tyranscript/tyran_runtime.h>
#include <tyranscript/tyran_function.h>
#include <tyranscript/tyran_property_iterator.h>

#include "../event/resource_updated.h"


static void info_add_function(nimbus_object_listener_info* self, tyran_value* function_context, const tyran_function* function)
{
	nimbus_object_listener_function* func_info = &self->functions[self->function_count++];
	tyran_value_copy(func_info->function_context, *function_context);
	func_info->function = function;
}


static void on_object_updated(nimbus_object_listener* self, tyran_object* o);

static void _on_resource_updated(void* _self, struct nimbus_event_read_stream* stream)
{
	nimbus_object_listener* self = _self;

	nimbus_resource_updated updated;

	nimbus_event_stream_read_type(stream, updated);
	TYRAN_LOG("Listener::_on_resource_updated.type:%d", updated.resource_type_id);
	if (self->object_type_id == updated.resource_type_id) {
		TYRAN_LOG("Got a match!");
		tyran_object* o;
		nimbus_event_stream_read_octets(stream, (u8t*)&o, sizeof(tyran_object*));
		on_object_updated(self, o);
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


	tyran_value return_value;
	for (int i=0; i<info->function_count; ++i) {
		nimbus_object_listener_function* func_info = &info->functions[i];
		tyran_runtime_push_call_ex(self->runtime, func_info->function, &func_info->function_context);
		tyran_runtime_execute(self->runtime, &return_value, 0);
	}
}

static void _dummy_update_2(void* _self2)
{
	nimbus_object_listener* c = _self2;

	call_event(c, c->frame_symbol);
}


void nimbus_object_listener_init(nimbus_object_listener* self, tyran_memory* memory, tyran_runtime* runtime)
{
	self->runtime = runtime;
	self->symbol_table = runtime->symbol_table;
	nimbus_update_init(&self->update, memory, _dummy_update_2, self, "script listener");
	nimbus_event_listener_init(&self->update.event_listener, self);
	nimbus_event_listener_listen(&self->update.event_listener, NIMBUS_EVENT_RESOURCE_UPDATED, _on_resource_updated);
	self->object_type_id = nimbus_resource_type_id_from_string("object");


	tyran_symbol_table_add(self->symbol_table, &self->frame_symbol, "Frame");

	self->info_max_count = 64;
	self->info_count = 0;
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


static void scan_for_listening_functions_on_object(nimbus_object_listener* self, tyran_value* object_value)
{
	tyran_object* o = tyran_value_object(object_value);
	tyran_property_iterator it;

	tyran_property_iterator_init(&it, o);

	tyran_symbol symbol;
	tyran_value* value;

	while (tyran_property_iterator_next(&it, &symbol, &value)) {
		const char* debug_key_string = tyran_symbol_table_lookup(self->symbol_table, &symbol);
		TYRAN_LOG("Property key:'%s'", debug_key_string);
		if (tyran_value_is_object(value)) {
			TYRAN_LOG("It is an object at least");
			if (tyran_value_is_function(value)) {
				if (debug_key_string[0] == 'o' && debug_key_string[1] == 'n') {
					TYRAN_LOG("found a listening func :)");
					add_listening_function(self, object_value, value, &debug_key_string[2]);
				}
			} else {
				scan_for_listening_functions_on_object(self, value);
			}
		}
	}

	tyran_property_iterator_free(&it);
}

static void on_object_updated(nimbus_object_listener* self, tyran_object* o)
{
	tyran_value v;
	tyran_value_set_object(v, o);
	scan_for_listening_functions_on_object(self, &v);
	tyran_value_release(v);
}
