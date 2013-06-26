#include "dependency_resolver.h"
#include <tyranscript/tyran_memory.h>
#include <tyranscript/tyran_clib.h>
#include <tyranscript/tyran_value.h>
#include <tyranscript/tyran_object.h>
#include <tyranscript/tyran_string.h>
#include <tyranscript/tyran_symbol_table.h>
#include <tyranscript/tyran_value_object.h>
#include "../event/resource_load.h"
#include "../resource/resource_cache.h"

static void load_resource(nimbus_dependency_resolver* self, nimbus_resource_id resource_id)
{
	self->loading_resources[self->loading_resources_count] = resource_id;
	self->loading_resources_count++;
	nimbus_resource_load_send(self->event_write_stream, resource_id);
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

static nimbus_resource_dependency_info* resource_depency_info_new(nimbus_dependency_resolver* self, nimbus_resource_id resource_id, tyran_value* target)
{
	TYRAN_LOG("Starting resource loading of id: %d", resource_id);
	TYRAN_ASSERT(!is_loading_in_progress(self, resource_id), "Can't start a loading job for a resource only in loading");
	TYRAN_ASSERT(nimbus_resource_cache_find(&self->resource_cache, resource_id) == 0, "Can't start resource loading job if resource already is ready?");

	nimbus_resource_dependency_info* info = &self->dependency_infos[self->dependency_info_count++];
	nimbus_resource_dependency_info_init(info, resource_id, target);
	
	return info;
}

static void load_resource_if_needed(nimbus_dependency_resolver* self, nimbus_resource_id resource_id)
{
	tyran_value* resource_value = nimbus_resource_cache_find(&self->resource_cache, resource_id);
	if (!resource_value) {
		if (!is_loading_in_progress(self, resource_id)) {
			load_resource(self, resource_id);
		} else {
			TYRAN_LOG("LOAD(%d) already loading - ignoring request", resource_id);
		}
	} else {
		TYRAN_LOG("LOAD(%d) already loaded", resource_id);
	}
}

static void inherit_resource(nimbus_dependency_resolver* self, nimbus_resource_dependency_info* info, tyran_value* target, nimbus_resource_id resource_id)
{
	nimbus_resource_dependency_info_inherit(info, resource_id);
	load_resource_if_needed(self, resource_id);
}

static void add_resource_reference(nimbus_dependency_resolver* self, nimbus_resource_dependency_info* info, tyran_value* target, nimbus_resource_id resource_id)
{
	nimbus_resource_dependency_info_add_resource(info, target, resource_id);
	load_resource_if_needed(self, resource_id);
}

static int request_inherits_and_references(nimbus_dependency_resolver* self, nimbus_resource_dependency_info* info, tyran_value* combine, tyran_value* v)
{
	// tyran_print_value("Checking out the inherits", o, 1);
	char value_string[128];
	int resources_that_are_loading = 0;

	tyran_object* o = tyran_value_object(v);
	tyran_object* combine_object = tyran_value_object(combine);
	
	for (int i = 0; i < o->property_count; ++i) {
		tyran_object_property* property = &o->properties[i];
		tyran_value* value = &property->value;

		if (tyran_value_is_string(value)) {
			tyran_string_to_c_str(value_string, 128, tyran_object_string(value->data.object));
			if (value_string[0] == '@') {
				nimbus_resource_id resource_id = nimbus_resource_id_from_string(&value_string[1]);
				tyran_value* resource = nimbus_resource_cache_find(&self->resource_cache, resource_id);
				if (resource != 0) {
					tyran_value_release(*value);
					tyran_value_copy(*value, *resource);
				} else {
					TYRAN_LOG("RESOURCE REFERENCE: '%s'", value_string);
					resources_that_are_loading++;
					add_resource_reference(self, info, value, resource_id);
				}
			} else if (value_string[0] == '#') {
				tyran_symbol symbol;
				tyran_symbol_table_add(self->symbol_table, &symbol, &value_string[1]);

				tyran_value looked_up_value;
				tyran_object_lookup_prototype(&looked_up_value, combine_object, &symbol);
				
				tyran_value_replace(*v, looked_up_value);
			} else {
				const char* key_string = tyran_symbol_table_lookup(self->symbol_table, &property->symbol);
				if (tyran_strcmp(key_string, "inherit") == 0) {
					TYRAN_LOG("Found inherit:'%s'", value_string);

					nimbus_resource_id resource_id = nimbus_resource_id_from_string(value_string);
					tyran_value* resource = nimbus_resource_cache_find(&self->resource_cache, resource_id);
					if (!resource) {
						resources_that_are_loading++;
						inherit_resource(self, info, v, resource_id);
					}
				}
			}
		} else if (tyran_value_is_object(value)) {
			if (!tyran_value_is_function(value)) {
				request_inherits_and_references(self, info, combine, value);
			}
		}
	}
	
	return resources_that_are_loading;
}

void nimbus_dependency_resolver_init(nimbus_dependency_resolver* self, struct tyran_memory* memory, tyran_symbol_table* symbol_table, struct nimbus_event_write_stream* stream)
{
	self->dependency_info_count = 0;
	self->symbol_table = symbol_table;
	self->event_write_stream = stream;
	nimbus_resource_cache_init(&self->resource_cache, memory);
}

static void check_if_resolved(nimbus_dependency_resolver* self, nimbus_resource_dependency_info* info);

static void check_if_someone_wants(nimbus_dependency_resolver* self, nimbus_resource_id resource_id, tyran_value* v)
{
	for (int i=0; i<self->dependency_info_count; ++i) {
		nimbus_resource_dependency_info* dependency_info = &self->dependency_infos[i];
		if (dependency_info->inherit_resource_id == resource_id) {
				tyran_value_object_set_prototype(dependency_info->target, v);
				dependency_info->inherit_resource_id = 0;
		}
		
		for (int j=0; j<dependency_info->resource_dependencies_count; ) {
			nimbus_resource_dependency* dependency = &dependency_info->resource_dependencies[j];
			if (dependency->resource_id == resource_id) {
				tyran_value_replace(*dependency->target, *v);
				nimbus_resource_dependency_info_delete_at(dependency_info, j);
			} else {
				++j;
			}
		}
	
		check_if_resolved(self, dependency_info);
	}
}

static void delete_dependency_info(nimbus_dependency_resolver* self, nimbus_resource_dependency_info* info)
{
	for (int i=0; i<self->dependency_info_count; ++i) {
		if (&self->dependency_infos[i] == info) {
			int index = i;
			nimbus_resource_dependency_info_free(info);
			tyran_memmove_type(nimbus_resource_dependency, &self->dependency_infos[index], &self->dependency_infos[index + 1], self->dependency_info_count - index - 1);
			self->dependency_info_count--;
			return;
		}
	}
	TYRAN_ERROR("Couldn't find dependency info to delete");
}

static void resource_resolved(nimbus_dependency_resolver* self, nimbus_resource_dependency_info* info)
{
	loading_done(self, info->resource_id);
	nimbus_resource_cache_add(&self->resource_cache, info->resource_id, info->target);
	check_if_someone_wants(self, info->resource_id, info->target);
	delete_dependency_info(self, info);
}

static void check_if_resolved(nimbus_dependency_resolver* self, nimbus_resource_dependency_info* info)
{
	if (nimbus_resource_dependency_info_is_satisfied(info)) {
		resource_resolved(self, info);
	}
}

void nimbus_dependency_resolver_object_loaded(nimbus_dependency_resolver* self, tyran_value* v, nimbus_resource_id resource_id)
{
	nimbus_resource_dependency_info* info = resource_depency_info_new(self, resource_id, v);
	request_inherits_and_references(self, info, v, v);
	check_if_resolved(self, info);
}

tyran_boolean nimbus_dependency_resolver_done(nimbus_dependency_resolver* self)
{
	return self->dependency_info_count == 0;
}
