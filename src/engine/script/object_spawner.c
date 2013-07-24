#include "object_spawner.h"

#include <tyranscript/tyran_object.h>
#include <tyranscript/tyran_mocha_api.h>
#include <tyranscript/tyran_property_iterator.h>
#include <tyranscript/tyran_symbol_table.h>

void nimbus_object_spawner_init(nimbus_object_spawner* self, tyran_runtime* runtime, tyran_object* object_to_spawn)
{
	self->runtime = runtime;
	self->object_to_spawn = object_to_spawn;
	self->symbol_table = self->runtime->symbol_table;
	self->associations_max_count = 64;
	self->associations_count = 0;
}

static void associate_components(nimbus_object_spawner* self, tyran_object* destination_component, tyran_object* source_component)
{
	TYRAN_ASSERT(self->associations_count < self->associations_max_count, "Too many associations");
	nimbus_object_spawner_association* association = &self->associations[self->associations_count++];
	association->destination_component = destination_component;
	association->source_component = source_component;
}

static void duplicate_component_objects(nimbus_object_spawner* self, tyran_object* destination_component, tyran_object* source_component)
{
	tyran_property_iterator it;
	tyran_value* value;

	tyran_symbol symbol;

	tyran_property_iterator_init_shallow(&it, source_component);

	while (tyran_property_iterator_next(&it, &symbol, &value)) {
		if (tyran_value_is_object(value)) {
			if (!tyran_value_is_function(value) && !tyran_value_program_specific(value)) {
				tyran_object* duplicate_component_object = tyran_object_new(self->runtime);
				tyran_value duplicate_component_object_value;
				tyran_value_set_object(duplicate_component_object_value, duplicate_component_object);
				tyran_object_set_prototype(duplicate_component_object, tyran_value_object(value));
				tyran_object_insert(destination_component, &symbol, &duplicate_component_object_value);
				const char* debug_key_string = tyran_symbol_table_lookup(self->symbol_table, &symbol);
			}
		}
	}

	tyran_property_iterator_free(&it);
}

static void duplicate_component(nimbus_object_spawner* self, tyran_object* destination_combine, tyran_symbol symbol, tyran_object* source_component)
{
	tyran_object* destination_component = tyran_object_new(self->runtime);

	tyran_object_set_prototype(destination_component, source_component);

	associate_components(self, destination_component, source_component);
	duplicate_component_objects(self, destination_component, source_component);

	tyran_value destination_component_value;
	tyran_value_set_object(destination_component_value, destination_component);
	tyran_object_insert(destination_combine, &symbol, &destination_component_value);
}

static void duplicate_components(nimbus_object_spawner* self, tyran_object* destination_combine, tyran_object* source_combine)
{
	tyran_property_iterator it;
	tyran_value* value;

	tyran_symbol symbol;

	tyran_property_iterator_init_shallow(&it, source_combine);

	while (tyran_property_iterator_next(&it, &symbol, &value)) {
		if (tyran_value_is_object(value)) {
			if (!tyran_value_is_function(value)) {
				duplicate_component(self, destination_combine, symbol, tyran_value_object(value));
			}
		}
	}

	tyran_property_iterator_free(&it);
}

static tyran_object* lookup_reference(nimbus_object_spawner* self, tyran_object* source_reference)
{
	for (int i=0; i<self->associations_count; ++i) {
		nimbus_object_spawner_association* association = &self->associations[i];
		if (association->source_component == source_reference) {
			return association->destination_component;
		}
	}

	return 0;
}

static void rewire_internal_references_on_component(nimbus_object_spawner* self, tyran_object* destination_component, tyran_object* source_component)
{
	tyran_property_iterator it;
	tyran_symbol symbol;
	tyran_value* reference_value;

	tyran_property_iterator_init_shallow(&it, source_component);
	while (tyran_property_iterator_next(&it, &symbol, &reference_value)) {
		if (tyran_value_is_object(reference_value)) {
			if (!tyran_value_is_function(reference_value)) {
				tyran_object* reference = tyran_value_object(reference_value);
				tyran_object* converted_reference = lookup_reference(self, reference);
				if (converted_reference) {
					tyran_value converted_reference_value;
					tyran_value_set_object(converted_reference_value, converted_reference);
					tyran_object_insert(destination_component, &symbol, &converted_reference_value);
				}
			}
		}
	}
	tyran_property_iterator_free(&it);
}

static void rewire_internal_references_on_combine(nimbus_object_spawner* self, tyran_object* destination_combine, tyran_object* source_combine)
{
	tyran_property_iterator it;

	tyran_property_iterator_init_shallow(&it, source_combine);

	tyran_symbol symbol;
	tyran_value* value;
	while (tyran_property_iterator_next(&it, &symbol, &value)) {
		if (tyran_value_is_object(value)) {
			if (!tyran_value_is_function(value)) {
				tyran_object* source_component = tyran_value_object(value);
				tyran_value destination_component_value;
				tyran_object_lookup(&destination_component_value, destination_combine, &symbol);
				tyran_object* destination_component = tyran_value_object(&destination_component_value);
				rewire_internal_references_on_component(self, destination_component, source_component);
			}
		}
	}
	tyran_property_iterator_free(&it);
}

tyran_object* nimbus_object_spawner_spawn(nimbus_object_spawner* self)
{
	tyran_object* destination_combine = tyran_object_new(self->runtime);
	duplicate_components(self, destination_combine, self->object_to_spawn);
	rewire_internal_references_on_combine(self, destination_combine, self->object_to_spawn);
	tyran_value destination_combine_value;
	tyran_value_set_object(destination_combine_value, destination_combine);

	return destination_combine;
}
