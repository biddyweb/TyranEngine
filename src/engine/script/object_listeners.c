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

#include "event_definition.h"

#include <tyran_engine/event/resource_load.h>


void nimbus_object_collection_init(nimbus_object_collection* self, struct tyran_memory* memory)
{
	self->count = 0;
	self->max_count = 1024;
	self->entries = TYRAN_MEMORY_CALLOC_TYPE_COUNT(memory, tyran_object*, self->max_count);
}

void nimbus_object_collection_add(nimbus_object_collection* self, tyran_object* o)
{
	self->entries[self->count++] = o;
}


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



static void nimbus_object_layers_add_layer(nimbus_object_listener* self, const char* name, struct tyran_memory* memory)
{
	nimbus_object_layer* layer = &self->layers[self->layers_count++];
	layer->name = tyran_strdup(memory, name);
}

static void setup_collection_for_event_definition(nimbus_object_listener* self, struct tyran_memory* memory, nimbus_event_definition* event_definition)
{
	TYRAN_LOG("Defining collection for type '%s'", event_definition->name);
	nimbus_object_collection_for_type* collection = &self->object_collection_for_types[self->object_collection_for_types_count++];
	collection->event_definition = event_definition;
	nimbus_object_collection_init(&collection->collection, memory);
}

static void setup_collections_for_event_definitions(nimbus_object_listener* self, struct tyran_memory* memory, nimbus_event_definition* event_definitions, int event_definition_count)
{
	for (int i=0; i<event_definition_count; ++i) {
		struct nimbus_event_definition* definition = &event_definitions[i];
		setup_collection_for_event_definition(self, memory, definition);
	}
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



static void trigger_spawn_in_layers(nimbus_object_listener* self, tyran_symbol type_name)
{
	for (int i=0; i<self->layers_count; ++i) {
		nimbus_object_layer* layer = &self->layers[i];
		nimbus_resource_id layer_specific_resource_id = resource_id_for_layer(layer->name, self->symbol_table, type_name);
		nimbus_resource_load_send(&self->update.event_write_stream, layer_specific_resource_id);
	}
}


static nimbus_object_collection* object_collection_for_type(nimbus_object_listener* self, tyran_symbol type_name)
{
	for (int i=0; i < self->object_collection_for_types_count; ++i) {
		nimbus_object_collection_for_type* collection = &self->object_collection_for_types[i];
		if (tyran_symbol_equal(&collection->event_definition->type_symbol, &type_name)) {
			return &collection->collection;
		}
	}

	return 0;
}

static void serialize_object_collection(nimbus_object_listener* self, nimbus_object_collection* collection, struct nimbus_event_definition* e)
{
	tyran_object** entries = collection->entries;
	for (int i=0; i<collection->count; ++i) {
		tyran_object* object = entries[i];
		nimbus_object_to_event_convert(&self->object_to_event, &self->update.event_write_stream, object, e);
	}
}

static void serialize_all(nimbus_object_listener* self)
{
	for (int i=0; i<self->object_collection_for_types_count; ++i) {
		nimbus_object_collection_for_type* type_collection = &self->object_collection_for_types[i];
		serialize_object_collection(self, &type_collection->collection, type_collection->event_definition);
	}
}

static void handle_type_object(nimbus_object_listener* self, tyran_object* o, tyran_symbol type_name, const char* type_name_string)
{
	TYRAN_LOG("Found type: '%s'", type_name_string);
	nimbus_decorate_object(o, self->memory);
	trigger_spawn_in_layers(self, type_name);

	nimbus_object_collection* collection = object_collection_for_type(self, type_name);
	if (collection) {
		nimbus_object_collection_add(collection, o);
	}
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

static void _update(void* _self)
{
	nimbus_object_listener* self = _self;

	call_event(self, self->frame_symbol);
	serialize_all(self);
}

void nimbus_object_listener_init(nimbus_object_listener* self, tyran_memory* memory, tyran_runtime* runtime, nimbus_event_definition* event_definitions, int event_definition_count)
{
	self->object_collection_for_types_count = 0;
	self->runtime = runtime;
	self->symbol_table = runtime->symbol_table;
	const int max_event_octets = 16 * 1024;
	nimbus_update_init_ex(&self->update, memory, _update, self, max_event_octets, "script object listener");
	nimbus_event_listener_init(&self->update.event_listener, self);
	nimbus_event_listener_listen(&self->update.event_listener, NIMBUS_EVENT_RESOURCE_UPDATED, _on_resource_updated);
	self->state_type_id = nimbus_resource_type_id_from_string("state");
	tyran_symbol_table_add(runtime->symbol_table, &self->type_symbol, "type");

	tyran_symbol_table_add(self->symbol_table, &self->frame_symbol, "Frame");

	nimbus_object_to_event_init(&self->object_to_event, memory, runtime->symbol_table);


	self->info_max_count = 64;
	self->info_count = 0;
	self->memory = memory;

	self->max_layers_count = 8;
	self->layers_count = 0;
	self->layers = TYRAN_MEMORY_CALLOC_TYPE_COUNT(memory, nimbus_object_layer, self->max_layers_count);

	nimbus_object_layers_add_layer(self, "render", memory);

	setup_collections_for_event_definitions(self, memory, event_definitions, event_definition_count);

	/*
		self->infos = TYRAN_MEMORY_CALLOC_TYPE_COUNT(memory, nimbus_object_listener_info, self->info_max_count);
		for (int i=0; i<self->info_max_count; ++i) {
			nimbus_object_listener_info* info = &self->infos[i];
			info->max_function_count = 64;
			info->functions = TYRAN_MEMORY_CALLOC_TYPE_COUNT(memory, nimbus_object_listener_function, info->max_function_count);
		}
	*/
}

