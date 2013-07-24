#include "object_listeners.h"
#include <tyranscript/tyran_mocha_api.h>
#include <tyranscript/tyran_object.h>
#include <tyranscript/tyran_symbol_table.h>
#include <tyranscript/tyran_runtime.h>
#include <tyranscript/tyran_function.h>
#include <tyranscript/tyran_property_iterator.h>

#include <tyranscript/debug/tyran_print_opcodes.h>

#include <tyran_engine/event/resource_updated.h>

#include <tyranscript/tyran_symbol_table.h>
#include "object_info.h"
#include "object_decorator.h"
#include "event_definition.h"
#include "object_spawner.h"
#include "track_info.h"
#include "../event/resource_load_state.h"

#include <tyran_engine/event/resource_load.h>
#include <tyran_engine/event/resource_load.h>
#include <tyran_engine/event/module_resource_updated.h>


static tyran_object* evaluate(nimbus_object_listener* self, const char* data)
{
	tyran_value new_object = tyran_mocha_api_create_object(self->mocha);
	tyran_object* object = tyran_value_object(&new_object);

	tyran_object_set_prototype(object, self->context);
	tyran_value temp_value;
	tyran_value_set_nil(temp_value);
	tyran_mocha_api_eval(self->mocha, &new_object, &temp_value, data);
	return tyran_value_object(&new_object);
}


static tyran_object* get_or_create_resource_object(nimbus_object_listener* self, nimbus_resource_id resource_id, int track_index)
{
	tyran_value new_object = tyran_mocha_api_create_object(self->mocha);
	tyran_object* object = tyran_value_object(&new_object);
	nimbus_object_info* info = nimbus_decorate_object(object, self->memory);
	info->is_module_resource = TYRAN_TRUE;
	info->track_index = track_index;

	return object;
}

static void add_object(nimbus_object_listener* self, nimbus_resource_id resource_id, nimbus_resource_type_id resource_type_id, tyran_object* o)
{
	nimbus_dependency_resolver_object_loaded(&self->dependency_resolver, o, resource_id, resource_type_id);
}



static void on_script_source_updated(nimbus_object_listener* self, struct nimbus_event_read_stream* stream, nimbus_resource_id resource_id, nimbus_resource_type_id resource_type_id, int payload_size)
{
	TYRAN_ASSERT(payload_size <= self->script_buffer_size, "Buffer too small for script. payload:%d max:%d", payload_size, self->script_buffer_size);

	nimbus_event_stream_read_octets(stream, self->script_buffer, payload_size);
	self->script_buffer[payload_size] = 0;

	// TYRAN_LOG("*** EVALUATE *** %d octet_size:%d", resource_id, payload_size);
	// TYRAN_LOG("SCRIPT:'%s'", self->script_buffer);
	tyran_object* o = evaluate(self, (const char*)self->script_buffer);

	add_object(self, resource_id, resource_type_id, o);
}



static void on_module_resource_updated(nimbus_object_listener* self, struct nimbus_event_read_stream* stream, nimbus_resource_id resource_id, int payload_size)
{
	int index;

	nimbus_event_stream_read_octets(stream, (u8t*)&index, sizeof(index));

	tyran_object* o = get_or_create_resource_object(self, resource_id, index);

	add_object(self, resource_id, self->module_resource_type_id, o);
}


static void _on_resource_load_state(void* _self, struct nimbus_event_read_stream* stream)
{
	nimbus_object_listener* self = _self;

	nimbus_resource_load_state load_state;
	nimbus_event_stream_read_type(stream, load_state);

	nimbus_resource_load_send(&self->update.event_write_stream, load_state.resource_id);

	self->waiting_for_state_resource_id = load_state.resource_id;
}

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

static tyran_object* find_resource(nimbus_object_listener* self, nimbus_resource_id layer_specific_resource_id)
{
	return 0;
}

static tyran_object* fetch_resource(nimbus_object_listener* self, nimbus_resource_id layer_specific_resource_id)
{
	tyran_object* o = find_resource(self, layer_specific_resource_id);
	if (!o) {
		nimbus_resource_load_send(&self->update.event_write_stream, layer_specific_resource_id);
	}

	return o;
}



static nimbus_layer_association* find_layer_association(nimbus_object_listener* self, tyran_object* source_object)
{
	for (int i=0; i<self->associations_count; ++i) {
		nimbus_layer_association* association = &self->associations[i];
		if (association->source_object == source_object) {
			return association;
		}
	}
	return 0;
}


static void nimbus_layer_association_init(nimbus_layer_association* self, struct tyran_memory* memory, nimbus_type_to_layers* type_to_layers, tyran_object* source_object)
{
	for (int i=0; i<32; ++i) {
		self->layer_objects[i] = 0;
	}

	self->source_object = source_object;
	self->type_to_layers = type_to_layers;
	nimbus_object_collection_init(&self->update_objects, memory);
}

static nimbus_layer_association* get_layer_association(nimbus_object_listener* self, nimbus_type_to_layers* type_to_layers, tyran_object* source_object)
{
	nimbus_layer_association* association = find_layer_association(self, source_object);
	if (!association) {
		TYRAN_ASSERT(self->associations_count < self->associations_max_count, "too many associations");
		association = &self->associations[self->associations_count++];
		nimbus_layer_association_init(association, self->memory, type_to_layers, source_object);
	}

	return association;
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
		tyran_value function_value;

		tyran_object_lookup_prototype(&function_value, object, &self->on_update_symbol);
		//if (!tyran_value_is_nil(&function_value)) {
		const tyran_function* on_update_function = tyran_value_function(&function_value);
		tyran_value this_value;
		tyran_value_set_object(this_value, object);
		tyran_runtime_push_call_ex_arguments(self->runtime, on_update_function, &this_value, &argument_value, 1);
		tyran_runtime_execute(self->runtime, &return_value, 0);
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

void nimbus_type_to_layers_init(nimbus_type_to_layers* self, tyran_symbol type_name)
{
	self->infos_count = 0;
	self->infos_max_count = 32;
	self->type_name = type_name;
}

void nimbus_type_to_layers_add(nimbus_type_to_layers* self, nimbus_resource_id layer_specific_resource_id)
{
	TYRAN_ASSERT(self->infos_count < self->infos_max_count, "Overwrite layers add");

	nimbus_type_to_layers_info* info = &self->infos[self->infos_count++];
	info->resource_id = layer_specific_resource_id;
	info->combine = 0;
}

static void add_spawned_object(nimbus_object_listener* self, nimbus_type_to_layers* type, tyran_object* spawned_object)
{
	get_layer_association(self, type, spawned_object);

}

static nimbus_type_to_layers* add_type_to_layers(nimbus_object_listener* self, tyran_object* o, tyran_symbol type_name)
{
	nimbus_type_to_layers* type_to_layer = &self->type_to_layers[self->type_to_layers_count++];
	nimbus_type_to_layers_init(type_to_layer, type_name);
	for (int i=0; i<self->layers_count; ++i) {
		nimbus_object_layer* layer = &self->layers[i];
		nimbus_resource_id layer_specific_resource_id = resource_id_for_layer(layer->name, self->symbol_table, type_name);
		nimbus_type_to_layers_add(type_to_layer, layer_specific_resource_id);
		nimbus_resource_load_send(&self->update.event_write_stream, layer_specific_resource_id);
	}

	return type_to_layer;
}

static nimbus_type_to_layers* find_type_to_layers(nimbus_object_listener* self, tyran_symbol type_name)
{
	for (int i=0; i<self->type_to_layers_count; ++i) {
		nimbus_type_to_layers* type_to_layer = &self->type_to_layers[i];
		if (tyran_symbol_equal(&type_to_layer->type_name, &type_name)) {
			return type_to_layer;
		}
	}

	return 0;
}

static nimbus_type_to_layers* get_type_to_layers(nimbus_object_listener* self, tyran_object* o, tyran_symbol type_name)
{
	nimbus_type_to_layers* type_to_layers = find_type_to_layers(self, type_name);
	if (!type_to_layers) {
		type_to_layers = add_type_to_layers(self, o, type_name);
	}

	return type_to_layers;
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

static nimbus_track_info* get_or_create_track_info(nimbus_object_listener* self, tyran_symbol type_name)
{
	nimbus_track_info* track_info = track_info_from_type(self, type_name);
	if (!track_info) {
		track_info = add_track_info(self, type_name);
	}

	return track_info;
}

static void handle_type_object(nimbus_object_listener* self, tyran_object* o, tyran_symbol type_name, const char* type_name_string)
{
	nimbus_object_info* info = nimbus_decorate_object(o, self->memory);
	if (!is_event_type(self, type_name)) {
		nimbus_type_to_layers* type_to_layers = get_type_to_layers(self, o, type_name);
		add_spawned_object(self, type_to_layers, o);
	} else {
		nimbus_track_info* track_info = get_or_create_track_info(self, type_name);
		info->track_index = nimbus_track_info_get_free_index(track_info);
	}
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
				scan_component(self, tyran_value_object(value), combine);
			}
		}
	}

	tyran_property_iterator_free(&it);
}

static tyran_object* spawn(nimbus_object_listener* self, tyran_object* combine)
{
	nimbus_object_spawner spawner;
	nimbus_object_spawner_init(&spawner, self->runtime, combine);
	tyran_object* spawned_combine = nimbus_object_spawner_spawn(&spawner);
	scan_combine(self, spawned_combine);

	return spawned_combine;
}

static void search_components_for_update_functions(nimbus_object_listener* self, nimbus_layer_association* association, tyran_object* combine)
{
	tyran_property_iterator it;

	tyran_property_iterator_init_shallow(&it, combine);

	tyran_symbol symbol;
	tyran_value* value;

	while (tyran_property_iterator_next(&it, &symbol, &value)) {
		if (tyran_value_is_object(value) && !tyran_value_is_function(value)) {
			tyran_object* object = tyran_value_object(value);
			tyran_value update_func_value;
			tyran_object_lookup_prototype(&update_func_value, object, &self->on_update_symbol);
			if (tyran_value_is_function(&update_func_value)) {
				nimbus_object_collection_add(&association->update_objects, object);
			}
		}
	}

	tyran_property_iterator_free(&it);

}

static void spawn_layer_objects_waiting_for_resource_id(nimbus_object_listener* self, nimbus_type_to_layers* layer, tyran_object* combine, int layer_index)
{
	for (int i=0; i<self->associations_count; ++i) {
		nimbus_layer_association* association = &self->associations[i];
		if (association->type_to_layers == layer) {
			TYRAN_ASSERT(association->layer_objects[layer_index] == 0, "Something bad happened when spawning");
			tyran_object* spawned_combine = spawn(self, combine);
			association->layer_objects[layer_index] = spawned_combine;
			search_components_for_update_functions(self, association, spawned_combine);
		}
	}
}


static void check_if_layer_resource(nimbus_object_listener* self, tyran_object* o, nimbus_resource_id resource_id)
{
	for (int i=0; i<self->type_to_layers_count; ++i) {
		nimbus_type_to_layers* layer = &self->type_to_layers[i];
		for (int j=0; j<layer->infos_count; ++j) {
			nimbus_type_to_layers_info* info = &layer->infos[j];
			if (info->resource_id == resource_id) {
				info->combine = o;
				spawn_layer_objects_waiting_for_resource_id(self, layer, info->combine, i);
			}
		}
	}
}


static void on_object_updated(nimbus_object_listener* self, tyran_object* o, nimbus_resource_id resource_id)
{
	if (self->waiting_for_state_resource_id == resource_id) {
		TYRAN_LOG("********* State %d is loaded!", resource_id);
		nimbus_resource_updated_send(&self->update.event_write_stream, resource_id, self->state_type_id, &o, sizeof(o));
	}
	check_if_layer_resource(self, o, resource_id);
}


static void on_state_updated(nimbus_object_listener* self, tyran_object* o, nimbus_resource_id resource_id)
{
	TYRAN_LOG("STATE loaded %d", resource_id);
	scan_combine(self, o);
}



static void _update(void* _self)
{
	nimbus_object_listener* self = _self;

	call_event(self, self->frame_symbol);
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


void nimbus_object_listener_init(nimbus_object_listener* self, tyran_memory* memory, struct tyran_mocha_api* mocha, struct tyran_object* context, nimbus_event_definition* event_definitions, int event_definition_count)
{
	self->object_collection_for_types_count = 0;
	self->runtime = mocha->default_runtime;
	self->mocha = mocha;
	self->symbol_table = self->runtime->symbol_table;
	const int max_event_octets = 16 * 1024;
	nimbus_update_init_ex(&self->update, memory, _update, self, max_event_octets, "script object listener");
	nimbus_event_listener_init(&self->update.event_listener, self);
	nimbus_event_listener_listen(&self->update.event_listener, NIMBUS_EVENT_RESOURCE_UPDATED, _on_resource_updated);
	nimbus_event_listener_listen(&self->update.event_listener, NIMBUS_EVENT_RESOURCE_LOAD_STATE, _on_resource_load_state);

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
	self->memory = memory;

	self->max_layers_count = 8;
	self->layers_count = 0;
	self->layers = TYRAN_MEMORY_CALLOC_TYPE_COUNT(memory, nimbus_object_layer, self->max_layers_count);

	self->track_infos_max_count = 32;
	self->track_infos = TYRAN_MEMORY_CALLOC_TYPE_COUNT(memory, nimbus_track_info, self->track_infos_max_count);
	self->track_infos_count = 0;

	self->context = context;
	self->script_buffer_size = 16 * 1024;
	self->script_buffer = TYRAN_MEMORY_ALLOC(memory, self->script_buffer_size, "Script buffer");
	self->waiting_for_state_resource_id = 0;


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

