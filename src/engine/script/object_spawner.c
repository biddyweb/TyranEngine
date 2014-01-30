#include "object_spawner.h"

#include <tyranscript/tyran_object.h>
#include <tyranscript/tyran_mocha_api.h>
#include <tyranscript/tyran_property_iterator.h>
#include <tyranscript/tyran_symbol_table.h>

#include <tyran_engine/event_definition/event_definition.h>
#include "object_info.h"

/*
static void print_object(nimbus_object_spawner* self, const char* description, tyran_object* object)
{
	tyran_value object_value;
	tyran_value_set_object(object_value, object);
	tyran_print_value(description, &object_value, 1, self->symbol_table);
	tyran_value_release(object_value);
}
*/

void nimbus_object_spawner_init(nimbus_object_spawner* self, tyran_runtime* runtime, nimbus_event_definition* defintions, int definitions_count, const tyran_object* object_to_spawn)
{
	self->runtime = runtime;
	self->object_to_spawn = object_to_spawn;
	self->symbol_table = self->runtime->symbol_table;
	self->associations_max_count = 64;
	self->associations_count = 0;
	self->definitions_count = definitions_count;
	self->defintions = defintions;
	tyran_symbol_table_add(runtime->symbol_table, &self->type_symbol, "type");
}

static void associate_components(nimbus_object_spawner* self, tyran_object* destination_component, const tyran_object* source_component)
{
	TYRAN_ASSERT(self->associations_count < self->associations_max_count, "Too many associations");
	nimbus_object_spawner_association* association = &self->associations[self->associations_count++];
	association->destination_component = destination_component;
	association->source_component = source_component;
}

static nimbus_event_definition* find_event_definition(nimbus_object_spawner* self, tyran_symbol type_symbol)
{
	for (int i=0; i<self->definitions_count; ++i) {
		nimbus_event_definition* definition = &self->defintions[i];
		if (tyran_symbol_equal(&type_symbol, &definition->type_symbol)) {
			return definition;
		}
	}

	return 0;
}

static tyran_boolean should_duplicate_component(nimbus_object_spawner* self, const tyran_object* o)
{
	const tyran_value* component_type_symbol_value;
	tyran_object_lookup(&component_type_symbol_value, o, &self->type_symbol);
	if (tyran_value_is_symbol(component_type_symbol_value)) {
		nimbus_event_definition* definition = find_event_definition(self, tyran_value_symbol(component_type_symbol_value));
		if (definition) {
			return definition->should_instantiate;
		}
	}

	return TYRAN_TRUE;
}


static void duplicate_component_objects(nimbus_object_spawner* self, tyran_object* destination_component, const tyran_object* source_component)
{
	tyran_property_iterator it;
	const tyran_value* value;

	tyran_symbol symbol;

	tyran_property_iterator_init_shallow(&it, source_component);

	while (tyran_property_iterator_next(&it, &symbol, &value)) {
		if (tyran_value_is_object_generic(value)) {
			nimbus_object_info* info = (nimbus_object_info*) tyran_value_program_specific(value);
			tyran_boolean is_module_resource = info != 0; // && info->is_module_resource;
			if (!is_module_resource && should_duplicate_component(self, tyran_value_object(value))) {
				tyran_object* duplicate_component_object = tyran_object_new(self->runtime);
				tyran_value duplicate_component_object_value;
				tyran_value_set_object(duplicate_component_object_value, duplicate_component_object);
				tyran_object_set_prototype(duplicate_component_object, tyran_value_object(value));
				tyran_object_insert(destination_component, &symbol, &duplicate_component_object_value);
				tyran_value_release(duplicate_component_object_value);
			}
		}
	}

	tyran_property_iterator_free(&it);
}

static void duplicate_component(nimbus_object_spawner* self, tyran_object* destination_combine, tyran_symbol symbol, const tyran_object* source_component)
{
	tyran_object* destination_component = tyran_object_new(self->runtime);
	tyran_value destination_component_value;
	tyran_value_set_object(destination_component_value, destination_component);

	tyran_object_set_prototype(destination_component, source_component);

	tyran_object_insert(destination_combine, &symbol, &destination_component_value);
	tyran_value_release(destination_component_value);

	associate_components(self, destination_component, source_component);
	duplicate_component_objects(self, destination_component, source_component);
}

static void duplicate_components(nimbus_object_spawner* self, tyran_object* destination_combine, const tyran_object* source_combine)
{
	tyran_property_iterator it;
	const tyran_value* value;

	tyran_symbol symbol;

	tyran_property_iterator_init_shallow(&it, source_combine);

	while (tyran_property_iterator_next(&it, &symbol, &value)) {
		if (tyran_value_is_object(value)) {
			if (should_duplicate_component(self, tyran_value_object(value))) {
				duplicate_component(self, destination_combine, symbol, tyran_value_object(value));
			}
		}
	}

	tyran_property_iterator_free(&it);
}

static tyran_object* lookup_reference(nimbus_object_spawner* self, const tyran_object* source_reference)
{
	for (int i=0; i<self->associations_count; ++i) {
		nimbus_object_spawner_association* association = &self->associations[i];
		if (association->source_component == source_reference) {
			return association->destination_component;
		}
	}

	return 0;
}

static void rewire_internal_references_on_component(nimbus_object_spawner* self, tyran_object* destination_component, const tyran_object* source_component)
{
	tyran_property_iterator it;
	tyran_symbol symbol;
	const tyran_value* reference_value;

	tyran_property_iterator_init_shallow(&it, source_component);
	while (tyran_property_iterator_next(&it, &symbol, &reference_value)) {
		if (tyran_value_is_object_generic(reference_value)) {
			const tyran_object* reference = tyran_value_object(reference_value);
			tyran_object* converted_reference = lookup_reference(self, reference);
			if (converted_reference) {
				tyran_value converted_reference_value;
				tyran_value_set_object(converted_reference_value, converted_reference);
				tyran_object_insert(destination_component, &symbol, &converted_reference_value);
				tyran_value_release(converted_reference_value);
			} else {
				TYRAN_LOG("No conversion for '%s'", tyran_symbol_table_lookup(self->symbol_table, &symbol))
			}
		}
	}
	tyran_property_iterator_free(&it);
}

static void rewire_internal_references_on_combine(nimbus_object_spawner* self, tyran_object* destination_combine, const tyran_object* source_combine)
{
	tyran_property_iterator it;

	tyran_property_iterator_init_shallow(&it, source_combine);

	tyran_symbol symbol;
	const tyran_value* value;
	while (tyran_property_iterator_next(&it, &symbol, &value)) {
		if (tyran_value_is_object_generic(value)) {
			const tyran_object* source_component = tyran_value_object(value);
			const tyran_value* destination_component_value;
			tyran_object_lookup(&destination_component_value, destination_combine, &symbol);
			if (tyran_value_is_object_generic(destination_component_value)) {
				tyran_object* destination_component = tyran_value_mutable_object((tyran_value*)destination_component_value);
				rewire_internal_references_on_component(self, destination_component, source_component);
			} else {

			}
		}
	}
	tyran_property_iterator_free(&it);
}

tyran_object* nimbus_object_spawner_spawn(nimbus_object_spawner* self)
{
	tyran_object* destination_combine = tyran_object_new(self->runtime);
	tyran_object_retain(destination_combine);
	tyran_object_set_prototype(destination_combine, self->object_to_spawn);
	//print_object(self, "destination_combine", destination_combine);
	duplicate_components(self, destination_combine, self->object_to_spawn);
	rewire_internal_references_on_combine(self, destination_combine, self->object_to_spawn);
	return destination_combine;
}
