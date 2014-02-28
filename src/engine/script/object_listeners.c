#include "object_listeners.h"
/*
#include <tyranscript/tyran_mocha_api.h>
#include <tyranscript/tyran_object.h>
#include <tyranscript/tyran_symbol_table.h>
#include <tyranscript/tyran_runtime.h>
#include <tyranscript/tyran_function.h>
#include <tyranscript/tyran_property_iterator.h>
#include <tyranscript/debug/tyran_print_opcodes.h>
#include <tyranscript/tyran_symbol_table.h>
#include <tyranscript/debug/tyran_runtime_debug.h>

#include <tyran_engine/event/resource_updated.h>
#include <tyran_engine/event/resource_load.h>
#include <tyran_engine/event/resource_load.h>
#include <tyran_engine/event/module_resource_updated.h>
#include <tyran_engine/event/unspawn_event.h>

#include "object_info.h"
#include <tyran_engine/script/object_decorator.h>
#include <tyran_engine/event_definition/event_definition.h>

#include "../event/resource_load_state.h"
#include <tyran_engine/event/execute_function.h>

#include "../base/nimbus_engine.h"

void nimbus_object_collection_init(nimbus_object_collection* self, struct tyran_memory* memory)
{
	self->count = 0;
	self->max_count = 1024;
	self->entries = TYRAN_MEMORY_CALLOC_TYPE_COUNT(memory, tyran_object*, self->max_count);
}

void nimbus_object_collection_add(nimbus_object_collection* self, tyran_object* o)
{
	TYRAN_ASSERT(self->count < self->max_count, "Overflow object collection");
	self->entries[self->count++] = o;
	// TYRAN_ASSERT(tyran_object_program_specific(o), "Must have info to be added and retained");
	TYRAN_OBJECT_RETAIN(o);
}

void nimbus_object_collection_remove(nimbus_object_collection* self, tyran_object* o)
{
	for (int i=0; i<self->count; ++i) {
		if (self->entries[i] == o) {
			tyran_object_release(o);
			self->count--;
			tyran_memmove_type(tyran_object*, &self->entries[i], &self->entries[i+1], self->count - i);
			return;
		}
	}
}

static void nimbus_layer_association_init(nimbus_layer_association* self, struct tyran_memory* memory, nimbus_type_to_layer_combines* type_to_layers, tyran_object* source_object)
{
	for (int i=0; i<32; ++i) {
		self->layer_objects[i] = 0;
	}

	self->source_object = source_object;
	self->type_to_layers = type_to_layers;
	nimbus_object_collection_init(&self->update_objects, memory);
}

static void nimbus_layer_association_init_free(nimbus_layer_association* self)
{
	for (int i=0; i<32; ++i) {
		tyran_object* layer_object = self->layer_objects[i];
		if (layer_object) {
			self->layer_objects[i] = 0;
			tyran_object_release(layer_object);
		}
	}
}

static void nimbus_layer_association_delete(nimbus_object_listener* self, nimbus_layer_association* association)
{
	nimbus_layer_association_init_free(association);
	for (int i=0; i<self->associations_count; ++i) {
		nimbus_layer_association* find_association = &self->associations[i];
		if (find_association == association) {
			self->associations_count--;
			tyran_memmove_type(nimbus_layer_association, &self->associations[i], &self->associations[i+1], self->associations_count - i);
			return;
		}
	}
}

static nimbus_resource_id resource_id_for_layer(const char* layer_name, tyran_symbol_table* symbol_table, tyran_symbol symbol)
{
	const char* type_name_string = tyran_symbol_table_lookup(symbol_table, &symbol);

	const int temp_buf_size = 128;
	char temp[temp_buf_size];
	tyran_strncpy(temp, temp_buf_size, layer_name, tyran_strlen(layer_name));
	tyran_strncat(temp, "/", temp_buf_size);
	tyran_strncat(temp, type_name_string, temp_buf_size);

	nimbus_resource_id layer_specific_resource_id = nimbus_resource_id_from_string(temp);

	return layer_specific_resource_id;
}

void nimbus_type_to_layers_init(nimbus_type_to_layer_combines* self, tyran_symbol type_name)
{
	self->infos_count = 0;
	self->infos_max_count = 32;
	self->type_name = type_name;
}

void nimbus_type_to_layers_add(nimbus_type_to_layer_combines* self, nimbus_resource_id layer_specific_resource_id)
{
	TYRAN_ASSERT(self->infos_count < self->infos_max_count, "Overwrite layers add");

	nimbus_type_to_layers_info* info = &self->infos[self->infos_count++];
	info->resource_id = layer_specific_resource_id;
	info->combine = 0;
}

static nimbus_type_to_layer_combines* add_type_to_layers(nimbus_object_listener* self, tyran_object* o, tyran_symbol type_name)
{
	nimbus_type_to_layer_combines* type_to_layer = &self->type_to_layers[self->type_to_layers_count++];
	nimbus_type_to_layers_init(type_to_layer, type_name);
	for (int i=0; i<self->layers_count; ++i) {
		nimbus_object_layer* layer = &self->layers[i];
		nimbus_resource_id layer_specific_resource_id = resource_id_for_layer(layer->name, self->symbol_table, type_name);
		nimbus_type_to_layers_add(type_to_layer, layer_specific_resource_id);
		nimbus_resource_type_id resource_type_id = nimbus_resource_type_id_from_string("object");
		nimbus_resource_load_send(&self->update.event_write_stream, layer_specific_resource_id, resource_type_id);
	}

	return type_to_layer;
}

static nimbus_type_to_layer_combines* find_type_to_layers(nimbus_object_listener* self, tyran_symbol type_name)
{
	for (int i=0; i<self->type_to_layers_count; ++i) {
		nimbus_type_to_layer_combines* type_to_layer = &self->type_to_layers[i];
		if (tyran_symbol_equal(&type_to_layer->type_name, &type_name)) {
			return type_to_layer;
		}
	}

	return 0;
}

static nimbus_type_to_layer_combines* get_type_to_layers(nimbus_object_listener* self, tyran_object* o, tyran_symbol type_name)
{
	nimbus_type_to_layer_combines* type_to_layers = find_type_to_layers(self, type_name);
	if (!type_to_layers) {
		type_to_layers = add_type_to_layers(self, o, type_name);
	}

	return type_to_layers;
}



static tyran_object* get_or_create_module_resource_object(nimbus_object_listener* self, nimbus_resource_id resource_id, int instance_index)
{
	tyran_value new_object = tyran_mocha_api_create_object(self->mocha);
	tyran_object* object = tyran_value_mutable_object(&new_object);
	nimbus_object_info* info = nimbus_decorate_object(object, self->memory);
	info->is_module_resource = TYRAN_TRUE;
	info->instance_index = instance_index;


	return object;
}

static void add_object(nimbus_object_listener* self, nimbus_resource_id resource_id, nimbus_resource_type_id resource_type_id, tyran_object* o)
{
	nimbus_dependency_resolver_object_loaded(&self->dependency_resolver, o, resource_id, resource_type_id);
}



static void on_module_resource_updated(nimbus_object_listener* self, struct nimbus_event_read_stream* stream, nimbus_resource_id resource_id, int payload_size)
{
	int instance_index;

	nimbus_event_stream_read_octets(stream, (u8t*)&instance_index, sizeof(instance_index));

	tyran_object* o = get_or_create_module_resource_object(self, resource_id, instance_index);
	// TYRAN_LOG("Found resource object: %d at index %d", resource_id, instance_index);

	add_object(self, resource_id, self->module_resource_type_id, o);
}


static void _on_resource_load_state(void* _self, struct nimbus_event_read_stream* stream)
{
	nimbus_object_listener* self = _self;

	nimbus_resource_load_state load_state;
	nimbus_event_stream_read_type(stream, load_state);

	nimbus_resource_load_send(&self->update.event_write_stream, load_state.resource_id, nimbus_resource_type_id_from_string("state") );

	self->waiting_for_state_resource_id = load_state.resource_id;
}

static void info_add_function(nimbus_object_listener_info* self, nimbus_combine_instance_id combine_instance_id, tyran_object* function_context, const tyran_function* function)
{
	TYRAN_ASSERT(self->function_count < self->max_function_count, "end of functions");
	nimbus_object_listener_function* func_info = &self->functions[self->function_count++];
	func_info->function_context = function_context;
	TYRAN_OBJECT_RETAIN(function_context);
	func_info->function = function;
	func_info->combine_instance_id = combine_instance_id;
}

static void info_delete_listeners_with_context(nimbus_object_listener_info* self, tyran_object* function_context)
{
	for (int i=0; i<self->function_count; ) {
		nimbus_object_listener_function* func_info = &self->functions[i];
		if (func_info->function_context == function_context) {
			self->function_count--;
			tyran_object_release(func_info->function_context);
			tyran_memmove_type(nimbus_object_listener_function, &self->functions[i], &self->functions[i+1], self->function_count - i);
		} else {
			++i;
		}
	}
}

static void delete_listeners_with_context(nimbus_object_listener* self, tyran_object* function_context)
{
	for (int i=0; i<self->info_count; ++i) {
		nimbus_object_listener_info* info = &self->infos[i];
		info_delete_listeners_with_context(info, function_context);
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
		info->max_function_count = 256;
		info->symbol = symbol;
		info->function_count = 0;
	}
	return info;
}

static void call_event(nimbus_object_listener* self, nimbus_combine_instance_id combine_instance_id, tyran_symbol symbol, struct tyran_value* arguments, int arguments_count)
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
		if (combine_instance_id != NIMBUS_COMBINE_INSTANCE_ID_NONE && func_info->combine_instance_id != combine_instance_id) {
			continue;
		}

#if defined OBJECT_LISTENERS_DEBUG
		TYRAN_LOG("UPDATE function:%p, function_context:%p", func_info->function, &func_info->function_context);
		// tyran_print_value("context", &func_info->function_context, 1, self->symbol_table);
		tyran_print_opcodes(func_info->function->data.opcodes, 0, func_info->function->constants);
#endif
		tyran_runtime_clear(self->runtime);
		tyran_value context_value;
		tyran_value_set_object(context_value, func_info->function_context);
		tyran_runtime_push_call_ex_arguments(self->runtime, func_info->function, &context_value, arguments, arguments_count);
		tyran_value_release(context_value);
		tyran_runtime_execute(self->runtime, &return_value, 0);
		tyran_value_release(return_value);
		tyran_runtime_clear(self->runtime);
	}
}

static void execute_function(nimbus_object_listener* self, nimbus_combine_instance_id instance_id, tyran_symbol function_symbol)
{
	call_event(self, instance_id, function_symbol, 0, 0);
}

static void _on_execute_function(void* _self, struct nimbus_event_read_stream* stream)
{
	nimbus_object_listener* self = _self;

	nimbus_event_execute_function* event;
	nimbus_event_stream_read_type_pointer(stream, event, nimbus_event_execute_function);
	execute_function(self, event->instance_id, event->function_symbol);
}

static void nimbus_object_layers_add_layer(nimbus_object_listener* self, const char* name, struct tyran_memory* memory)
{
	nimbus_object_layer* layer = &self->layers[self->layers_count++];
	layer->name = tyran_strdup(memory, name);
}

static tyran_boolean is_event_type(nimbus_object_listener* self, tyran_symbol type_symbol)
{
	for (int i=0; i<self->object_collection_for_types_count; ++i) {
		nimbus_object_collection_for_type* collection = &self->object_collection_for_types[i];
		if (tyran_symbol_equal(&collection->event_definition->type_symbol, &type_symbol)) {
			return TYRAN_TRUE;
		}
	}
	return TYRAN_FALSE;
}

static void setup_collection_for_event_definition(nimbus_object_listener* self, struct tyran_memory* memory, nimbus_event_definition* event_definition)
{
	nimbus_object_collection_for_type* collection = &self->object_collection_for_types[self->object_collection_for_types_count++];
	collection->event_definition = event_definition;
	nimbus_object_collection_init(&collection->collection, memory);
}

static nimbus_event_definition* event_definition_for_type_symbol(nimbus_object_listener* self, const tyran_symbol* type_symbol)
{
	for (int i=0; i<self->event_definitions_count; ++i) {
		struct nimbus_event_definition* definition = &self->event_definitions[i];
		if (tyran_symbol_equal(&definition->type_symbol, type_symbol)) {
			return definition;
		}
	}

	return 0;
}

static void setup_collections_for_event_definitions(nimbus_object_listener* self, struct tyran_memory* memory, nimbus_event_definition* event_definitions, int event_definition_count)
{
	for (int i=0; i<event_definition_count; ++i) {
		struct nimbus_event_definition* definition = &event_definitions[i];
		setup_collection_for_event_definition(self, memory, definition);
	}
}

static void add_listening_function(nimbus_object_listener* self, nimbus_combine_instance_id combine_instance_id, tyran_object* function_context, const tyran_value* function, const char* event_name)
{
	tyran_symbol symbol;

	tyran_symbol_table_add(self->symbol_table, &symbol, event_name);
	nimbus_object_listener_info* info = info_from_symbol(self, symbol);
	// TYRAN_LOG("Found listening function '%s' on combine %d", event_name, combine_instance_id);
	info_add_function(info, combine_instance_id, function_context, tyran_value_function(function));
}

static void scan_for_listening_functions_on_object(nimbus_object_listener* self, tyran_object* o, tyran_object* combine, nimbus_combine_instance_id combine_instance_id)
{
	tyran_property_iterator it;

	tyran_property_iterator_init(&it, o);

	tyran_symbol symbol;
	const tyran_value* value;

	while (tyran_property_iterator_next(&it, &symbol, &value)) {
		const char* debug_key_string = tyran_symbol_table_lookup(self->symbol_table, &symbol);
		if (tyran_value_is_function(value)) {
			if (debug_key_string[0] == 'o' && debug_key_string[1] == 'n') {
				add_listening_function(self, combine_instance_id, o, value, &debug_key_string[2]);
			}
		}
	}

	tyran_property_iterator_free(&it);
}

typedef struct all_objects {
	const u8t* objects;
	int object_count;
} all_objects;

static nimbus_track_info* add_track_info(nimbus_object_listener* self, tyran_symbol type_name)
{
	nimbus_track_info* info = &self->track_infos[self->track_infos_count++];
	nimbus_track_info_init(info, self->memory, type_name);
	return info;
}

static nimbus_track_info* track_info_from_type(nimbus_object_listener* self, tyran_symbol type_name)
{
	for (int i=0; i<self->track_infos_count; ++i) {
		nimbus_track_info* info = &self->track_infos[i];
		if (tyran_symbol_equal(&info->type_symbol, &type_name)) {
			return info;
		}
	}

	return 0;
}

static void _on_all(void* _self, struct nimbus_event_read_stream* stream)
{
	nimbus_object_listener* self = _self;
	nimbus_event_type_id event_type_id = stream->event_type_id;

	for (int i=0; i < self->event_definitions_count; ++i) {
		nimbus_event_definition* definition = &self->event_definitions[i];
		if (definition->event_type_id == event_type_id) {
			if (definition->is_module_to_script) {
				tyran_value arguments[8];
				int argument_count = nimbus_event_to_arguments_convert(&self->arguments_converter, arguments, 8, stream, definition);
				call_event(self, NIMBUS_COMBINE_INSTANCE_ID_NONE, definition->type_symbol, arguments, argument_count);
				for (int argument_index = 0; argument_index < argument_count; ++argument_index) {
					tyran_value_release(arguments[argument_index]);
				}
			} else if (definition->is_module_to_script_objects) {
				nimbus_track_info* track = track_info_from_type(self, definition->struct_symbol);
				if (!track) {
					return;
				}
				const all_objects* e;
				nimbus_event_stream_read_type_pointer(stream, e, all_objects);
				tyran_object** objects = track->objects;
				for (int instance_index = 0; instance_index < e->object_count; ++instance_index) {
					const u8t* p = e->objects + instance_index * definition->struct_size;
					tyran_object* o = objects[instance_index];
					if (!o) {
						continue;
					}
					nimbus_event_to_object_convert(&self->object_converter, o, p, definition);
				}
			}
			return;
		}
	}
}

static nimbus_layer_association* find_layer_association(nimbus_object_listener* self, const tyran_object* source_object)
{
	for (int i=0; i<self->associations_count; ++i) {
		nimbus_layer_association* association = &self->associations[i];
		if (association->source_object == source_object) {
			return association;
		}
	}
	return 0;
}

static nimbus_track_info* get_or_create_track_info(nimbus_object_listener* self, tyran_symbol type_name)
{
	nimbus_track_info* track_info = track_info_from_type(self, type_name);
	if (!track_info) {
		track_info = add_track_info(self, type_name);
	}

	return track_info;
}

static tyran_object* spawn(nimbus_object_listener* self, const tyran_object* combine);

static void search_components_for_update_functions(nimbus_object_listener* self, nimbus_layer_association* association, tyran_object* combine)
{
	tyran_property_iterator it;

	tyran_property_iterator_init_shallow(&it, combine);

	tyran_symbol symbol;
	const tyran_value* value;

	while (tyran_property_iterator_next(&it, &symbol, &value)) {
		if (tyran_value_is_object_generic(value)) {
			tyran_object* object = tyran_value_mutable_object((tyran_value*)value);
			const tyran_value* update_func_value;
			tyran_object_lookup_prototype(&update_func_value, object, &self->on_update_symbol);
			if (tyran_value_is_function(update_func_value)) {
				nimbus_object_collection_add(&association->update_objects, object);
			}
		}
	}

	tyran_property_iterator_free(&it);

}

static void spawn_layer_object(nimbus_object_listener* self, nimbus_layer_association* association, int layer_index, tyran_object* combine, nimbus_resource_id combine_resource_id)
{
	TYRAN_ASSERT(association->layer_objects[layer_index] == 0, "Something bad happened when spawning");
	tyran_object* spawned_combine = spawn(self, combine);
	tyran_print_object("spawned layer object", spawned_combine, 1, self->symbol_table);
	association->layer_objects[layer_index] = spawned_combine;
	search_components_for_update_functions(self, association, spawned_combine);
}

static void spawn_available_layer_objects(nimbus_object_listener* self, nimbus_layer_association* association, nimbus_type_to_layer_combines* type_to_layers)
{
	for (int layer_index = 0; layer_index < type_to_layers->infos_count; ++layer_index) {
		nimbus_type_to_layers_info* info = &type_to_layers->infos[layer_index];
		if (info->combine) {
			spawn_layer_object(self, association, layer_index, info->combine, info->resource_id);
		}
	}
}

static nimbus_layer_association* get_layer_association(nimbus_object_listener* self, nimbus_type_to_layer_combines* type_to_layers, tyran_object* source_object)
{
	nimbus_layer_association* association = find_layer_association(self, source_object);
	if (!association) {
		TYRAN_ASSERT(self->associations_count < self->associations_max_count, "too many associations");
		association = &self->associations[self->associations_count++];
		nimbus_layer_association_init(association, self->memory, type_to_layers, source_object);
	}

	return association;
}

static void add_spawned_object(nimbus_object_listener* self, nimbus_type_to_layer_combines* type, tyran_object* spawned_object)
{
	nimbus_layer_association* association = get_layer_association(self, type, spawned_object);
	spawn_available_layer_objects(self, association, type);
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

static void handle_type_object(nimbus_object_listener* self, tyran_object* o, nimbus_combine_instance_id combine_instance_id, tyran_symbol type_name, const char* type_name_string)
{
	nimbus_object_info* info = tyran_object_program_specific(o);
	if (!info) {
		info = nimbus_decorate_object(o, self->memory);
	}
	info->combine_instance_id = combine_instance_id;
	if (!is_event_type(self, type_name)) {
		nimbus_type_to_layer_combines* type_to_layers = get_type_to_layers(self, o, type_name);
		TYRAN_ASSERT(type_to_layers != 0, "Couldn't get layer for type name: '%s'", type_name_string);
		add_spawned_object(self, type_to_layers, o);
	} else {
		nimbus_track_info* track_info = get_or_create_track_info(self, type_name);
		info->event_definition = event_definition_for_type_symbol(self, &type_name);
		info->instance_index = nimbus_track_info_get_free_index(track_info, o);
		info->is_spawned = TYRAN_TRUE;
	}
	nimbus_object_collection* collection = object_collection_for_type(self, type_name);
	if (collection) {
		TYRAN_ASSERT(o->program_specific != 0, "Must have program specific!!");
		nimbus_object_collection_add(collection, o);
	}
}

static void check_for_type_on_component(nimbus_object_listener* self, tyran_object* component, nimbus_combine_instance_id combine_instance_id)
{
	const tyran_value* found_value;
	tyran_object_lookup_prototype(&found_value, component, &self->type_symbol);
	if (tyran_value_is_symbol(found_value)) {
		tyran_symbol type_value = tyran_value_symbol(found_value);
		const char* type_value_string = tyran_symbol_table_lookup(self->symbol_table, &type_value);
		handle_type_object(self, component, combine_instance_id, type_value, type_value_string);
	}
}

static void scan_component(nimbus_object_listener* self, tyran_object* component, tyran_object* combine, nimbus_combine_instance_id combine_instance_id)
{
	scan_for_listening_functions_on_object(self, component, combine, combine_instance_id);
	nimbus_object_info* combine_info = (nimbus_object_info*) tyran_object_program_specific(combine);
	tyran_value combine_value;
	tyran_value_set_number(combine_value, combine_instance_id);
	tyran_symbol combine_symbol;
	tyran_symbol_table_add(self->symbol_table, &combine_symbol, "combine_instance_id");
	tyran_object_insert(component, &combine_symbol, &combine_value);
	check_for_type_on_component(self, component, combine_info->combine_instance_id);
}

static void scan_combine(nimbus_object_listener* self, tyran_object* combine, nimbus_combine_instance_id combine_instance_id)
{
	tyran_property_iterator it;

	tyran_property_iterator_init(&it, combine);

	tyran_symbol symbol;
	const tyran_value* value;

	while (tyran_property_iterator_next(&it, &symbol, &value)) {
		if (tyran_value_is_object_generic(value)) {
			scan_component(self, tyran_value_mutable_object((tyran_value*)value), combine, combine_instance_id);
		}
	}

	tyran_property_iterator_free(&it);
}

static nimbus_combine_instance_id generate_combine_instance_id(nimbus_object_listener* self)
{
	nimbus_combine_instance_id id = nimbus_bit_array_reserve_free_index(&self->combine_instance_id_array);

	return id;
}

static nimbus_combine_instance_id assign_combine_instance_id(nimbus_object_listener* self, tyran_object* spawned_combine)
{
	nimbus_combine_instance_id id = generate_combine_instance_id(self);
	self->combine_instances[id] = spawned_combine;
	TYRAN_OBJECT_RETAIN(spawned_combine);
	return id;
}


static void on_state_updated(nimbus_object_listener* self, tyran_object* o, nimbus_resource_id resource_id)
{
	nimbus_combine_instance_id combine_instance_id = assign_combine_instance_id(self, o);
	nimbus_object_info* info = nimbus_decorate_object(o, self->memory);
	info->combine_instance_id = combine_instance_id;
	scan_combine(self, o, combine_instance_id);
}

static void update_layer_association(nimbus_object_listener* self, nimbus_layer_association* association)
{
	tyran_value argument_value;
	tyran_value_set_object(argument_value, association->source_object);

	tyran_value return_value;

	for (int i=0; i<association->update_objects.count; ++i) {
		tyran_object* object = association->update_objects.entries[i];
		if (!object) {
			return;
		}
		tyran_runtime_clear(self->runtime);
		const tyran_value* function_value;

		tyran_object_lookup_prototype(&function_value, object, &self->on_update_symbol);
		//if (!tyran_value_is_nil(&function_value)) {

		const tyran_function* on_update_function = tyran_value_function(function_value);
		tyran_value this_value;
		tyran_value_set_object(this_value, object);
		tyran_runtime_push_call_ex_arguments(self->runtime, on_update_function, &this_value, &argument_value, 1);
		tyran_value_release(this_value);
		tyran_runtime_execute(self->runtime, &return_value, 0);
		tyran_value_release(return_value);
		tyran_runtime_clear(self->runtime);
		//}
	}
}

static void update_layer_associations(nimbus_object_listener* self)
{
	for (int i=0; i<self->associations_count; ++i) {
		nimbus_layer_association* association = &self->associations[i];
		update_layer_association(self, association);
	}
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

static void send_unspawn(nimbus_object_listener* self, nimbus_object_info* component_info)
{
	nimbus_track_info* track_info = track_info_from_type(self, component_info->event_definition->type_symbol);
	// TYRAN_LOG("Send Unspawn %d name:%s", component_info->instance_index, component_info->event_definition->name);
	nimbus_track_info_delete_index(track_info, component_info->instance_index);

	nimbus_event_unspawn_send(&self->update.event_write_stream, component_info->event_definition->unspawn_event_type_id, component_info->instance_index);
}

void nimbus_object_listener_on_delete(nimbus_object_listener* self, tyran_object* object_to_be_deleted)
{
	delete_listeners_with_context(self, object_to_be_deleted);

	nimbus_object_info* info = tyran_object_program_specific(object_to_be_deleted);
	if (info->event_definition) {
		nimbus_object_collection* collection = object_collection_for_type(self, info->event_definition->type_symbol);
		nimbus_object_collection_remove(collection, object_to_be_deleted);
	}
	if (info && info->instance_index != -1) {
		send_unspawn(self, info);
	}
	if (info && info->event_definition) {
		nimbus_object_collection* collection = object_collection_for_type(self, info->event_definition->type_symbol);
		nimbus_object_collection_remove(collection, object_to_be_deleted);
	}
	if (info->is_spawned_combine) {
		nimbus_bit_array_delete_index(&self->combine_instance_id_array, info->combine_instance_id);

		tyran_property_iterator it;

		tyran_property_iterator_init_shallow(&it, object_to_be_deleted);

		tyran_symbol symbol;
		const tyran_value* value;

		while (tyran_property_iterator_next(&it, &symbol, &value)) {
			if (tyran_value_is_object(value)) {
				const char* debug_key_string = tyran_symbol_table_lookup(self->symbol_table, &symbol);
				TYRAN_LOG("COMPONENT:'%s'", debug_key_string);
				nimbus_layer_association* association = find_layer_association(self, tyran_value_object(value));
				if (association) {
					nimbus_layer_association_delete(self, association);
				}
			}
		}

		tyran_property_iterator_free(&it);
	}
}

void unspawn(nimbus_object_listener* self, nimbus_combine_instance_id combine_instance_id)
{
	tyran_object* combine_instance = self->combine_instances[combine_instance_id];
	tyran_object_release(combine_instance);
	self->combine_instances[combine_instance_id] = 0;
}

static tyran_object* spawn(nimbus_object_listener* self, const tyran_object* combine)
{
	nimbus_object_spawner spawner;
	nimbus_object_spawner_init(&spawner, self->runtime, self->event_definitions, self->event_definitions_count, combine);
	tyran_object* spawned_combine = nimbus_object_spawner_spawn(&spawner);
	nimbus_combine_instance_id combine_instance_id = assign_combine_instance_id(self, spawned_combine);
	nimbus_object_info* info = nimbus_decorate_object(spawned_combine, self->memory);
	info->is_spawned_combine = TYRAN_TRUE;
	info->combine_instance_id = combine_instance_id;
#if 0
	tyran_value o_value;
	tyran_value_set_object(o_value, spawned_combine);
	tyran_print_value("Spawned!", &o_value, 1, self->symbol_table);
	tyran_value_release(o_value);
#endif
	scan_combine(self, spawned_combine, combine_instance_id);


	return spawned_combine;
}

static void spawn_layer_objects_waiting_for_resource_id(nimbus_object_listener* self, nimbus_type_to_layer_combines* layer, tyran_object* combine, nimbus_resource_id resource_id, int layer_index)
{
	for (int i=0; i<self->associations_count; ++i) {
		nimbus_layer_association* association = &self->associations[i];
		if (association->type_to_layers == layer) {
			spawn_layer_object(self, association, layer_index, combine, resource_id);
		}
	}
}

static void check_if_layer_resource(nimbus_object_listener* self, tyran_object* o, nimbus_resource_id resource_id)
{
	for (int i=0; i<self->type_to_layers_count; ++i) {
		nimbus_type_to_layer_combines* layer = &self->type_to_layers[i];
		for (int layer_index=0; layer_index<layer->infos_count; ++layer_index) {
			nimbus_type_to_layers_info* info = &layer->infos[layer_index];
			if (info->resource_id == resource_id) {
				info->combine = o;
				spawn_layer_objects_waiting_for_resource_id(self, layer, info->combine, info->resource_id, layer_index);
			}
		}
	}
}

static void on_object_updated(nimbus_object_listener* self, tyran_object* o, nimbus_resource_id resource_id)
{
	if (self->waiting_for_state_resource_id == resource_id) {
		nimbus_resource_updated_send(&self->update.event_write_stream, resource_id, self->state_type_id, &o, sizeof(o));
	}
	check_if_layer_resource(self, o, resource_id);
}

static void _update(void* _self)
{
	nimbus_object_listener* self = _self;

	call_event(self, NIMBUS_COMBINE_INSTANCE_ID_NONE, self->frame_symbol, 0, 0);
	update_layer_associations(self);
	serialize_all(self);
}

static void _on_resource_updated(void* _self, struct nimbus_event_read_stream* stream)
{
	nimbus_object_listener* self = _self;

	nimbus_resource_updated updated;

	nimbus_event_stream_read_type(stream, updated);
	if (self->wire_object_type_id == updated.resource_type_id || self->script_object_type_id == updated.resource_type_id) {
		on_script_source_updated(self, stream, updated.resource_id, updated.resource_type_id, updated.payload_size);
	} else if (self->object_type_id == updated.resource_type_id) {
		tyran_object* o;
		nimbus_event_stream_read_octets(stream, (u8t*)&o, sizeof(o));
		on_object_updated(self, o, updated.resource_id);
	} else if (self->module_resource_type_id == updated.resource_type_id) {
		on_module_resource_updated(self, stream, updated.resource_id, updated.payload_size);
	} else if (updated.resource_type_id == self->state_type_id) {
		tyran_object* o;
		nimbus_event_stream_read_octets(stream, (u8t*)&o, sizeof(tyran_object*));
		on_state_updated(self, o, updated.resource_id);
	}
}

tyran_object* nimbus_object_listener_spawn(nimbus_object_listener* self, const tyran_object* combine)
{
	//TYRAN_ASSERT(combine->property_count >= 1, "Must have property counts");
#if 1
	tyran_value before_value;
	tyran_value_set_object(before_value, (tyran_object*)combine);
	tyran_print_value("before spawn", &before_value, 1, self->symbol_table);
	tyran_value_release(before_value);
#endif
	tyran_object* spawned_object = spawn(self, combine);
	nimbus_object_info* info = tyran_object_program_specific(spawned_object);
	TYRAN_ASSERT(info != 0, "Spawned must have program specific");
	TYRAN_ASSERT(info->is_spawned_combine, "Must be a combine");
	TYRAN_ASSERT(!info->is_spawned_component, "Should not be a component");

#if 1
	tyran_value after_value;
	tyran_value_set_object(after_value, (tyran_object*)spawned_object);
	tyran_print_value("after spawn", &after_value, 1, self->symbol_table);
	tyran_value_release(after_value);
#endif

	// TYRAN_ASSERT(spawned_object->property_count >= 1, "Must have property counts");
	return spawned_object;
}

void nimbus_object_listener_unspawn(nimbus_object_listener* self, nimbus_combine_instance_id combine)
{
	unspawn(self, combine);
}

TYRAN_RUNTIME_CALL_FUNC(on_call_event)
{
	nimbus_object_info* info = (nimbus_object_info*) tyran_value_program_specific(func);
	nimbus_engine* engine = runtime->program_specific_context;
	nimbus_object_listener* listener = &engine->object_listener;

	const nimbus_event_definition* event_definition = info->event_definition;
	TYRAN_LOG("on_call_event '%s'", event_definition->name);

	tyran_object* data_object = tyran_object_new(runtime);
	for (int i=0; i < event_definition->properties_count; ++i) {
		const nimbus_event_definition_property* property = &event_definition->properties[i];
		tyran_object_insert(data_object, &property->symbol, &arguments[i]);
	}

	nimbus_object_to_event_convert(&listener->object_to_event, &listener->update.event_write_stream, data_object, event_definition);

	return 0;
}

static void add_function_for_event_definition(nimbus_object_listener* self, const nimbus_event_definition* definition, tyran_object* events_object)
{
	tyran_value* function_object_value = tyran_function_object_new_callback(self->runtime, on_call_event);
	nimbus_object_info* info = nimbus_decorate_object(tyran_value_mutable_object(function_object_value), self->memory);
	info->event_definition = definition;
	tyran_object_insert(events_object, &definition->type_symbol, function_object_value);
}

static void setup_functions_for_event_definitions(nimbus_object_listener* self, struct tyran_memory* memory, const nimbus_event_definition* event_definitions, int event_definition_count)
{
	tyran_value events_object_value = tyran_mocha_api_create_object(self->mocha);

	tyran_value_object_insert_c_string_key(self->runtime, self->runtime->global, "events", &events_object_value);

	for (int i=0; i<event_definition_count; ++i) {
		const nimbus_event_definition* definition = &event_definitions[i];
		if (!definition->has_index) {
			add_function_for_event_definition(self, definition, tyran_value_mutable_object(&events_object_value));
		}
	}
}

void nimbus_object_listener_init(nimbus_object_listener* self, tyran_memory* memory, struct tyran_mocha_api* mocha, struct tyran_object* context, nimbus_event_definition* event_definitions, int event_definition_count)
{
	self->memory = memory;
	self->event_definitions = event_definitions;
	self->event_definitions_count = event_definition_count;

	self->object_collection_for_types_count = 0;
	self->runtime = mocha->default_runtime;
	self->mocha = mocha;
	self->symbol_table = self->runtime->symbol_table;
	tyran_symbol_table_add(self->symbol_table, &self->type_symbol, "type");
	const int max_event_octets = 64 * 1024;
	nimbus_update_init_ex(&self->update, memory, _update, self, max_event_octets, "script object listener");
	nimbus_event_listener_init(&self->update.event_listener, self);
	nimbus_event_listener_listen(&self->update.event_listener, NIMBUS_EVENT_RESOURCE_UPDATED, _on_resource_updated);
	nimbus_event_listener_listen(&self->update.event_listener, NIMBUS_EVENT_RESOURCE_LOAD_STATE, _on_resource_load_state);
	nimbus_event_listener_listen(&self->update.event_listener, NIMBUS_EVENT_EXECUTE_FUNCTION, _on_execute_function);
	nimbus_event_listener_listen_to_all(&self->update.event_listener, _on_all);

	nimbus_dependency_resolver_init(&self->dependency_resolver, memory, self->symbol_table, &self->update.event_write_stream);

	self->script_object_type_id = nimbus_resource_type_id_from_string("oes");
	self->wire_object_type_id = nimbus_resource_type_id_from_string("oec");
	self->object_type_id = nimbus_resource_type_id_from_string("object");
	self->state_type_id = nimbus_resource_type_id_from_string("state");
	self->module_resource_type_id = nimbus_resource_type_id_from_string("module_resource");

	tyran_symbol_table_add(self->symbol_table, &self->type_symbol, "type");
	tyran_symbol_table_add(self->symbol_table, &self->frame_symbol, "Frame");
	tyran_symbol_table_add(self->symbol_table, &self->on_update_symbol, "onUpdate");

	nimbus_object_to_event_init(&self->object_to_event, memory, self->symbol_table);

	self->associations_max_count = 512;
	self->associations = TYRAN_MEMORY_CALLOC_TYPE_COUNT(memory, nimbus_layer_association, self->associations_max_count);
	self->associations_count = 0;
	self->info_max_count = 64;
	self->info_count = 0;

	self->max_layers_count = 8;
	self->layers_count = 0;
	self->layers = TYRAN_MEMORY_CALLOC_TYPE_COUNT(memory, nimbus_object_layer, self->max_layers_count);

	self->track_infos_max_count = 32;
	self->track_infos = TYRAN_MEMORY_CALLOC_TYPE_COUNT(memory, nimbus_track_info, self->track_infos_max_count);
	self->track_infos_count = 0;

	self->context = context;
	self->waiting_for_state_resource_id = 0;

	self->combine_instances = TYRAN_MEMORY_CALLOC_TYPE_COUNT(memory, tyran_object*, 1024);

	nimbus_bit_array_init(&self->combine_instance_id_array, memory, 1024);

	nimbus_event_to_arguments_init(&self->arguments_converter, self->symbol_table, self->runtime);
	nimbus_event_to_object_init(&self->object_converter, self->symbol_table, self->runtime);

	nimbus_object_layers_add_layer(self, "render", memory);
	nimbus_object_layers_add_layer(self, "audio", memory);
	setup_collections_for_event_definitions(self, memory, event_definitions, event_definition_count);
	setup_functions_for_event_definitions(self, memory, event_definitions, event_definition_count);
}
*/
