#include "script_component_parser.h"

#include <tyranscript/tyran_symbol_table.h>
#include <tyranscript/tyran_symbol.h>
#include <tyranscript/tyran_value.h>
#include <tyranscript/tyran_object.h>
#include <tyranscript/tyran_string.h>
#include <tyranscript/tyran_symbol_table.h>
#include <tyranscript/tyran_property_iterator.h>

#include <tyran_engine/state/component_definition.h>
#include <tyran_engine/state/combine.h>
#include <tyran_engine/state/component.h>
#include <tyran_engine/event/intra_reference.h>
#include <tyran_engine/module/modules.h>

#include "script_property_parser.h"

static tyran_symbol lookup_type(struct tyran_symbol_table* symbol_table, const struct tyran_object* component_object)
{
	const tyran_value* type_value;
	tyran_symbol type_symbol;
	tyran_symbol_table_add(symbol_table, &type_symbol, "type");
	tyran_object_lookup(&type_value, component_object, &type_symbol);

	return tyran_value_symbol(type_value);
}

static void parse_property(nimbus_script_component_parser* self, nimbus_component* component, tyran_symbol symbol, const tyran_value* value)
{
	nimbus_script_property_parser_parse_property(&self->property_parser, component, symbol, value);
}

static void iterate_component(nimbus_script_component_parser* self, nimbus_modules* modules, nimbus_combine* combine, const tyran_object* component_object, tyran_symbol component_name)
{
	tyran_symbol type_symbol = lookup_type(self->symbol_table, component_object);
	
	tyran_symbol type_string_symbol;
	tyran_symbol_table_add(self->symbol_table, &type_string_symbol, "type");
	
	const nimbus_component_definition* component_definition = nimbus_modules_component_definition_from_type(modules, type_symbol);

	nimbus_component* component = nimbus_combine_create_component(combine, component_name, component_definition);

	tyran_property_iterator it;
	tyran_property_iterator_init_shallow(&it, component_object);

	tyran_symbol symbol;
	const tyran_value* value;

	while (tyran_property_iterator_next(&it, &symbol, &value)) {
		const char* debug_key_string = tyran_symbol_table_lookup(self->symbol_table, &symbol);
		TYRAN_LOG("Property: %s", debug_key_string);
		if (!tyran_symbol_equal(&symbol, &type_string_symbol)) {
			parse_property(self, component, symbol, value);
		}
	}

	tyran_property_iterator_free(&it);
}

void nimbus_script_component_parser_init(nimbus_script_component_parser* self, struct nimbus_modules* modules, tyran_symbol_table* symbol_table, struct nimbus_combine* combine, const struct tyran_object* component_script_object, tyran_symbol component_name)
{
	TYRAN_ASSERT(combine->state != 0, "Combine must have a state!");
	self->symbol_table = symbol_table;
	script_property_parser_init(&self->property_parser, symbol_table);
	iterate_component(self, modules, combine, component_script_object, component_name);
}
