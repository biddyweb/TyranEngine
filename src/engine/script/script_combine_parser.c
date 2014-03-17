#include "script_combine_parser.h"

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

static void iterate_component(nimbus_script_combine_parser* self, nimbus_modules* modules, nimbus_combine* combine, const tyran_object* component_script_object, tyran_symbol component_name)
{
	nimbus_script_component_parser_init(&self->component_parser, modules, self->symbol_table, combine, component_script_object, component_name);
}

static void iterate_combine(nimbus_script_combine_parser* self, nimbus_modules* modules, nimbus_combine* combine, const tyran_object* combine_script_object)
{
	tyran_property_iterator it;
	tyran_property_iterator_init_shallow(&it, combine_script_object);

	tyran_symbol symbol;
	const tyran_value* value;

	while (tyran_property_iterator_next(&it, &symbol, &value)) {
		const char* debug_key_string = tyran_symbol_table_lookup(self->symbol_table, &symbol);
		TYRAN_LOG("Component: %s", debug_key_string);
		const tyran_object* component_object = tyran_value_object(value);
		iterate_component(self, modules, combine, component_object, symbol);
	}

	tyran_property_iterator_free(&it);
}

static void parse_combine(nimbus_script_combine_parser* self, nimbus_modules* modules, nimbus_combine* combine, const tyran_object* combine_script_object, nimbus_resource_id resource_id)
{
	iterate_combine(self, modules, combine, combine_script_object);
}

void nimbus_script_combine_parser_init(nimbus_script_combine_parser* self, nimbus_modules* modules, tyran_symbol_table* symbol_table, nimbus_combine* combine, const tyran_object* combine_script_object, nimbus_resource_id resource_id)
{
	self->symbol_table = symbol_table;
	parse_combine(self, modules, combine, combine_script_object, resource_id);
}
