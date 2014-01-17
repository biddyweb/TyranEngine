#include "dependency_resolver.h"
#include <tyranscript/tyran_memory.h>
#include <tyranscript/tyran_clib.h>
#include <tyranscript/tyran_value.h>
#include <tyranscript/tyran_object.h>
#include <tyranscript/tyran_string.h>
#include <tyranscript/tyran_symbol_table.h>
#include <tyranscript/tyran_value_object.h>
#include <tyran_core/event/event_stream.h>
#include <tyran_engine/event/resource_load.h>
#include <tyran_engine/event/resource_updated.h>
#include "../resource/resource_cache.h"
#include <tyranscript/tyran_property_iterator.h>


static void load_resource(nimbus_dependency_resolver* self, nimbus_resource_id resource_id, nimbus_resource_type_id resource_type_id)
{
	TYRAN_ASSERT(self->loading_resources_count < self->loading_resources_max_count, " Too many loading resources");
	self->loading_resources[self->loading_resources_count] = resource_id;
	self->loading_resources_count++;
	nimbus_resource_load_send(self->event_write_stream, resource_id, resource_type_id);
}

static void send_resource_update(nimbus_event_write_stream* out_event_stream, nimbus_resource_id resource_id, nimbus_resource_type_id type_id, tyran_object* object)
{
	nimbus_resource_updated_send(out_event_stream, resource_id, type_id, &object, sizeof(tyran_object*));
}

static tyran_boolean is_loading_in_progress(nimbus_dependency_resolver* self, nimbus_resource_id resource_id)
{
	for (int i=0; i<self->loading_resources_count; ++i) {
		if (self->loading_resources[i] == resource_id) {
			return TYRAN_TRUE;
		}
	}

	return TYRAN_FALSE;
}

static void loading_done(nimbus_dependency_resolver* self, nimbus_resource_id resource_id)
{
	for (int i=0; i<self->loading_resources_count; ++i) {
		if (self->loading_resources[i] == resource_id) {
			tyran_memmove_type(nimbus_resource_id, &self->loading_resources[i], &self->loading_resources[i + 1], self->loading_resources_count - i - 1);
			self->loading_resources_count--;
			return;
		}
	}

}

static nimbus_resource_dependency_info* resource_depency_info_new(nimbus_dependency_resolver* self, nimbus_resource_id resource_id, tyran_object* target)
{
	// TYRAN_ASSERT(!is_loading_in_progress(self, resource_id), "Can't start a loading job for a resource only in loading");
	TYRAN_ASSERT(nimbus_resource_cache_find(&self->resource_cache, resource_id) == 0, "Can't start resource loading job if resource already is ready?");
	TYRAN_ASSERT(self->dependency_info_count < self->dependency_info_max_count, "too many dependency infos");
	nimbus_resource_dependency_info* info = &self->dependency_infos[self->dependency_info_count++];
	nimbus_resource_dependency_info_init(info, resource_id, target);

	return info;
}

static void load_resource_if_needed(nimbus_dependency_resolver* self, nimbus_resource_id resource_id, nimbus_resource_type_id resource_type_id)
{
	tyran_object* resource_value = nimbus_resource_cache_find(&self->resource_cache, resource_id);
	if (!resource_value) {
		if (!is_loading_in_progress(self, resource_id)) {
			load_resource(self, resource_id, resource_type_id);
		} else {
		}
	} else {
	}
}

static void inherit_resource(nimbus_dependency_resolver* self, nimbus_resource_dependency_info* info, tyran_value* target, nimbus_resource_id resource_id)
{
	nimbus_resource_dependency_info_inherit(info, target, resource_id);
	nimbus_resource_type_id resource_type_id = nimbus_resource_type_id_from_string("script");
	load_resource_if_needed(self, resource_id, resource_type_id);
}

static void add_resource_reference(nimbus_dependency_resolver* self, nimbus_resource_dependency_info* info, tyran_value* target, nimbus_resource_id resource_id, nimbus_resource_type_id resource_type_id)
{
	nimbus_resource_dependency_info_add_resource(info, target, resource_id);
	load_resource_if_needed(self, resource_id, resource_type_id);
}

static void check_resource_reference(nimbus_dependency_resolver* self, nimbus_resource_dependency_info* info, tyran_value* value, const char* value_string, const tyran_symbol* symbol)
{
	nimbus_resource_id resource_id = nimbus_resource_id_from_string(&value_string[1]);
	TYRAN_LOG("Reference '%s' -> %d", value_string, resource_id);
	tyran_object* resource = nimbus_resource_cache_find(&self->resource_cache, resource_id);
	if (resource != 0) {
		TYRAN_LOG("Found it in cache, setting it");
		tyran_value_replace_object(*value, resource);
	} else {
		const char* key_string = tyran_symbol_table_lookup(self->symbol_table, symbol);
		nimbus_resource_type_id resource_type_id;

		const char* resource_type_string = "object";

		if (tyran_str_equal(key_string, "image")) {
			resource_type_string = "image";
		} else if (tyran_str_equal(key_string, "fragment")) {
			resource_type_string = "fragment";
		} else if (tyran_str_equal(key_string, "vertex")) {
			resource_type_string = "vertex";
		} else if (tyran_str_equal(key_string, "skeleton")) {
			resource_type_string = "skeleton";
		} else if (tyran_str_equal(key_string, "wave")) {
			resource_type_string = "wave";
		}

		resource_type_id = nimbus_resource_type_id_from_string(resource_type_string);
		add_resource_reference(self, info, value, resource_id, resource_type_id);
	}
}

static void check_combine_reference(nimbus_dependency_resolver* self, const tyran_object* combine, tyran_value* value, const char* value_string)
{
	tyran_symbol symbol;

	tyran_symbol_table_add(self->symbol_table, &symbol, &value_string[1]);

	const tyran_value* looked_up_value;
	tyran_object_lookup_prototype(&looked_up_value, combine, &symbol);

	tyran_value_replace(*value, *looked_up_value);
}

static tyran_boolean check_string_value(nimbus_dependency_resolver* self, nimbus_resource_dependency_info* info, tyran_object* combine, tyran_value* value, const tyran_symbol* symbol)
{
	char value_string[128];
	tyran_string_to_c_str(value_string, 128, tyran_object_string(value->data.object));

	switch (value_string[0]) {
		case '@':
			check_resource_reference(self, info, value, value_string, symbol);
			return TYRAN_TRUE;
		case '#':
			check_combine_reference(self, combine, value, value_string);
			return TYRAN_TRUE;
	}
	return TYRAN_FALSE;
}

static void check_property_name(nimbus_dependency_resolver* self, nimbus_resource_dependency_info* info, tyran_value* property_value, const tyran_symbol* symbol, const tyran_value* value)
{
	const char* key_string = tyran_symbol_table_lookup(self->symbol_table, symbol);
	if (tyran_strcmp(key_string, "inherit") == 0) {
		char value_string[128];
		tyran_string_to_c_str(value_string, 128, tyran_object_string(value->data.object));

		nimbus_resource_id resource_id = nimbus_resource_id_from_string(value_string);
		tyran_object* resource = nimbus_resource_cache_find(&self->resource_cache, resource_id);
		TYRAN_LOG("Inherit '%s' -> %d", value_string, resource_id);
		if (!resource) {
			inherit_resource(self, info, property_value, resource_id);
		}
	}

}

static void check_inherits_and_reference_on_object(nimbus_dependency_resolver* self, nimbus_resource_dependency_info* info, tyran_value* property_value, tyran_object* combine)
{
	tyran_property_iterator it;

	const tyran_object* o = tyran_value_object(property_value);

	tyran_property_iterator_init(&it, o);

	const tyran_value* value;
	tyran_symbol symbol;
	while (tyran_property_iterator_next(&it, &symbol, &value)) {
		if (tyran_value_is_string(value)) {
			check_string_value(self, info, combine, (tyran_value*)value, &symbol);
		} else {
			check_property_name(self, info, property_value, &symbol, value);
		}
	}
}

static int request_inherits_and_references(nimbus_dependency_resolver* self, nimbus_resource_dependency_info* info, tyran_object* combine)
{
	tyran_property_iterator it;

	tyran_property_iterator_init(&it, combine);

	const tyran_value* value;
	tyran_symbol symbol;

	while (tyran_property_iterator_next(&it, &symbol, &value)) {
		if (tyran_value_is_object_generic(value)) {
			check_inherits_and_reference_on_object(self, info, (tyran_value*)value, combine);
		}
	}

	tyran_property_iterator_free(&it);

	return 0;
}

void nimbus_dependency_resolver_init(nimbus_dependency_resolver* self, struct tyran_memory* memory, tyran_symbol_table* symbol_table, struct nimbus_event_write_stream* stream)
{
	self->dependency_info_count = 0;
	self->loading_resources_count = 0;
	self->symbol_table = symbol_table;
	self->event_write_stream = stream;
	self->memory = memory;
	self->loading_resources_max_count = sizeof(self->loading_resources) / sizeof(nimbus_resource_id);
	self->dependency_info_max_count = sizeof(self->dependency_infos) / sizeof(nimbus_resource_dependency_info);
	self->object_type_id = nimbus_resource_type_id_from_string("object");
	self->wire_object_type_id = nimbus_resource_type_id_from_string("oec");
	nimbus_resource_cache_init(&self->resource_cache, memory);
}

static tyran_boolean check_if_resolved(nimbus_dependency_resolver* self, nimbus_resource_dependency_info* info);

static void check_if_someone_wants(nimbus_dependency_resolver* self, nimbus_resource_id resource_id, tyran_object* v)
{
	for (int i=0; i<self->dependency_info_count; ) {
		nimbus_resource_dependency_info* dependency_info = &self->dependency_infos[i];
		TYRAN_ASSERT(dependency_info->target != 0, "Target gone bad");

		for (int j=0; j<dependency_info->resource_dependencies_count; ) {
			nimbus_resource_dependency* dependency = &dependency_info->resource_dependencies[j];
			if (dependency->resource_id == resource_id) {
				if (dependency->is_inherit) {
					tyran_object* object_to_manipulate = tyran_value_mutable_object(dependency->target);
					tyran_object_set_prototype(object_to_manipulate, v);
				} else {
					tyran_value_replace_object(*dependency->target, v);
				}
				nimbus_resource_dependency_info_delete_at(dependency_info, j);
			} else {
				++j;
			}
		}

		if (check_if_resolved(self, dependency_info)) {
			i = 0;
		} else {
			++i;
		}
	}
}

static void delete_dependency_info(nimbus_dependency_resolver* self, nimbus_resource_dependency_info* info)
{
	for (int i=0; i<self->dependency_info_count; ++i) {
		if (&self->dependency_infos[i] == info) {
			int index = i;
			nimbus_resource_dependency_info_free(info);
			tyran_memmove_type(nimbus_resource_dependency_info, &self->dependency_infos[index], &self->dependency_infos[index + 1], self->dependency_info_count - index - 1);
			self->dependency_info_count--;
			return;
		}
	}
	TYRAN_ERROR("Couldn't find dependency info to delete");
}

static void resource_resolved(nimbus_dependency_resolver* self, nimbus_resource_dependency_info* info)
{
	TYRAN_LOG("Resolved '%s'", nimbus_resource_id_debug_name(info->resource_id));
	loading_done(self, info->resource_id);
	nimbus_resource_cache_add(&self->resource_cache, info->resource_id, info->target);
	nimbus_resource_id resource_id = info->resource_id;
	tyran_object* target = info->target;
	delete_dependency_info(self, info);
	TYRAN_ASSERT(target != 0, "Target is bad");
	send_resource_update(self->event_write_stream, resource_id, self->object_type_id, target);
	check_if_someone_wants(self, resource_id, target);
}

static tyran_boolean check_if_resolved(nimbus_dependency_resolver* self, nimbus_resource_dependency_info* info)
{
	if (nimbus_resource_dependency_info_is_satisfied(info)) {
		resource_resolved(self, info);
		return TYRAN_TRUE;
	} else {
		TYRAN_LOG("'%s' not ready. Waiting for...", nimbus_resource_id_debug_name(info->resource_id));
		for (int i=0; i<info->resource_dependencies_count; ++i) {
			nimbus_resource_dependency* dependency = &info->resource_dependencies[i];
			TYRAN_LOG(" '%s'", nimbus_resource_id_debug_name(dependency->resource_id));
		}
	}

	return TYRAN_FALSE;
}

void nimbus_dependency_resolver_object_loaded(nimbus_dependency_resolver* self, tyran_object* v, nimbus_resource_id resource_id, nimbus_resource_type_id resource_type_id)
{
	nimbus_resource_dependency_info* info = resource_depency_info_new(self, resource_id, v);
	if (resource_type_id == self->wire_object_type_id) {
		request_inherits_and_references(self, info, v);
	}
	check_if_resolved(self, info);
}

