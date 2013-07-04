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
	TYRAN_ASSERT(self->loading_resources_count < self->loading_resources_max_count, " Too many loading resources");
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

	TYRAN_SOFT_ERROR("Couldn't find that we were loading (%d)", resource_id);
}

static nimbus_resource_dependency_info* resource_depency_info_new(nimbus_dependency_resolver* self, nimbus_resource_id resource_id, tyran_value* target)
{
	TYRAN_LOG("Starting dependency tracking of resource: %d", resource_id);
	// TYRAN_ASSERT(!is_loading_in_progress(self, resource_id), "Can't start a loading job for a resource only in loading");
	TYRAN_ASSERT(nimbus_resource_cache_find(&self->resource_cache, resource_id) == 0, "Can't start resource loading job if resource already is ready?");
	TYRAN_ASSERT(self->dependency_info_count < self->dependency_info_max_count, "too many dependency infos");
	nimbus_resource_dependency_info* info = &self->dependency_infos[self->dependency_info_count++];
	nimbus_resource_dependency_info_init(info, resource_id, target);

	return info;
}

static void load_resource_if_needed(nimbus_dependency_resolver* self, nimbus_resource_id resource_id)
{
	tyran_value* resource_value = nimbus_resource_cache_find(&self->resource_cache, resource_id);
	if (!resource_value) {
		if (!is_loading_in_progress(self, resource_id)) {
			TYRAN_LOG("LOAD(%d) need to request it", resource_id);
			load_resource(self, resource_id);
		} else {
			TYRAN_LOG("LOAD(%d) already loading - ignoring request", resource_id);
		}
	} else {
		TYRAN_LOG("LOAD(%d) already loaded in cache!?", resource_id);
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

static int request_inherits_and_references(nimbus_dependency_resolver* self, nimbus_resource_dependency_info* info, tyran_value* combine, tyran_value* v, int depth)
{

	char tabs[200];
	int octet_count = depth * 5;
	tyran_memset_type_n(tabs, '.', octet_count);
	tabs[octet_count] = 0;


	// tyran_print_value("Checking out the inherits", o, 1);
	char value_string[128];
	int resources_that_are_loading = 0;

	tyran_object* o = tyran_value_object(v);
	TYRAN_LOG("%s Checking inherits and references. Property count:%d", tabs, o->property_count);

	for (int i = 0; i < o->property_count; ++i) {
		tyran_object_property* property = &o->properties[i];
		tyran_value* value = &property->value;
		const char* debug_key_string = tyran_symbol_table_lookup(self->symbol_table, &property->symbol);
		TYRAN_LOG("%s Property(%d) key:'%s'", tabs, i, debug_key_string);

		if (tyran_value_is_string(value)) {
			tyran_string_to_c_str(value_string, 128, tyran_object_string(value->data.object));
			if (value_string[0] == '@') {
				nimbus_resource_id resource_id = nimbus_resource_id_from_string(&value_string[1]);
				tyran_value* resource = nimbus_resource_cache_find(&self->resource_cache, resource_id);
				if (resource != 0) {
					TYRAN_LOG("%s RESOURCE REFERENCE (found): '%s'", tabs, value_string);
					tyran_value_replace(*value, *resource);
				} else {
					TYRAN_LOG("%s RESOURCE REFERENCE (dependency): '%s'", tabs, value_string);
					resources_that_are_loading++;
					add_resource_reference(self, info, value, resource_id);
				}
			} else if (value_string[0] == '#') {
				TYRAN_LOG("%s COMBINE REFERENCE: '%s'", tabs, value_string);
				tyran_value symbol_value;
				tyran_symbol symbol;

				tyran_symbol_table_add(self->symbol_table, &symbol, &value_string[1]);
				tyran_value_set_symbol(symbol_value, symbol);

				tyran_value looked_up_value;
				tyran_value_object_lookup_prototype(&looked_up_value, combine, &symbol_value);

				tyran_value_replace(*value, looked_up_value);
			} else {
				const char* key_string = tyran_symbol_table_lookup(self->symbol_table, &property->symbol);
				if (tyran_strcmp(key_string, "inherit") == 0) {
					TYRAN_LOG("%s INHERIT: '%s'", tabs, value_string);

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
				request_inherits_and_references(self, info, combine, value, depth + 1);
			}
		}
	}

	return resources_that_are_loading;
}

void nimbus_dependency_resolver_init(nimbus_dependency_resolver* self, struct tyran_memory* memory, tyran_symbol_table* symbol_table, struct nimbus_event_write_stream* stream)
{
	self->dependency_info_count = 0;
	self->loading_resources_count = 0;
	self->symbol_table = symbol_table;
	self->event_write_stream = stream;
	self->loading_resources_max_count = sizeof(self->loading_resources) / sizeof(nimbus_resource_id);
	self->dependency_info_max_count = sizeof(self->dependency_infos) / sizeof(nimbus_resource_dependency_info);
	nimbus_resource_cache_init(&self->resource_cache, memory);
}

static tyran_boolean check_if_resolved(nimbus_dependency_resolver* self, nimbus_resource_dependency_info* info);

static void check_if_someone_wants(nimbus_dependency_resolver* self, nimbus_resource_id resource_id, tyran_value* v)
{
	for (int i=0; i<self->dependency_info_count; ) {
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

		if (check_if_resolved(self, dependency_info)) {
			i = i;
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
	nimbus_resource_id resource_id = info->resource_id;
	tyran_value* target = info->target;
	delete_dependency_info(self, info);
	check_if_someone_wants(self, resource_id, target);
}

static tyran_boolean check_if_resolved(nimbus_dependency_resolver* self, nimbus_resource_dependency_info* info)
{
	if (nimbus_resource_dependency_info_is_satisfied(info)) {
		resource_resolved(self, info);
		return TYRAN_TRUE;
	}

	return TYRAN_FALSE;
}

void nimbus_dependency_resolver_object_loaded(nimbus_dependency_resolver* self, tyran_value* v, nimbus_resource_id resource_id)
{
	nimbus_resource_dependency_info* info = resource_depency_info_new(self, resource_id, v);
	request_inherits_and_references(self, info, v, v, 0);
	check_if_resolved(self, info);
}

tyran_boolean nimbus_dependency_resolver_done(nimbus_dependency_resolver* self)
{
	TYRAN_LOG("DONE count:%d", self->dependency_info_count);
	if (self->dependency_info_count > 0) {
		nimbus_resource_dependency_info* dependency_info = &self->dependency_infos[0];
		for (int j=0; j<dependency_info->resource_dependencies_count; ) {
			nimbus_resource_dependency* dependency = &dependency_info->resource_dependencies[j];
			TYRAN_LOG("waiting for dependency:%d", dependency->resource_id);
		}
		TYRAN_LOG("waiting for inherit:%d", dependency_info->inherit_resource_id);
	}
	return self->dependency_info_count == 0;
}
