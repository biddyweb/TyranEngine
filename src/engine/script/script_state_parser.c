#include "script_state_parser.h"

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

#include "../state/state.h"
#include "script_combine_parser.h"


static void parse_combine(nimbus_script_state_parser* self, const tyran_object* combine_script_object)
{
	nimbus_combine* combine = nimbus_state_create_combine(self->state);

	nimbus_script_combine_parser combine_parser;
	nimbus_script_combine_parser_init(&combine_parser, self->modules, self->symbol_table, combine, combine_script_object, self->resource_id);
}

static void iterate_state_object(nimbus_script_state_parser* self, const tyran_object* state_script_object)
{
	tyran_property_iterator it;
	tyran_property_iterator_init_shallow(&it, state_script_object);

	tyran_symbol symbol;
	const tyran_value* value;

	while (tyran_property_iterator_next(&it, &symbol, &value)) {
		const char* debug_key_string = tyran_symbol_table_lookup(self->symbol_table, &symbol);
		TYRAN_LOG("State Combine: %s", debug_key_string);
		const tyran_object* combine_object = tyran_value_object(value);
		parse_combine(self, combine_object);
	}

	tyran_property_iterator_free(&it);
}

void nimbus_script_state_parser_init(nimbus_script_state_parser* self, struct nimbus_modules* modules, struct tyran_symbol_table* symbol_table, struct nimbus_state* state, const struct tyran_object* state_script_object, nimbus_resource_id resource_id)
{
	self->resource_id = resource_id;
	self->modules = modules;
	self->symbol_table = symbol_table;
	self->state = state;
	iterate_state_object(self, state_script_object);
}
