#include "script_combine.h"

static tyran_symbol lookup_type(const tyran_symbol_table* symbol_table, tyran_object* component_object)
{
	const tyran_value* type_value;
	tyran_symbol type_symbol;
	tyran_symbol_table_add(self->mocha_api.symbol_table, &type_symbol, "type");
	tyran_object_lookup(&type_value, component_object, &type_symbol);

	return tyran_value_symbol(type_value);
}

static const nimbus_component_definition* component_definition_from_type(nimbus_script_combine_parser* self)
{
	return 0;
}

static void value_to_property(nimbus_component* component, const nimbus_component_definition_property* property, const tyran_value* value, nimbus_property_reader* reader)
{
	void* data = (void*) (component->component_data + propert->offset_in_struct);
	const tyran_object* o;
	switch (property->type) {
		case NIMBUS_COMPONENT_DEFINITION_FLOAT:
			*(float *) data) = tyran_value_number(value);
			break;
		case NIMBUS_COMPONENT_DEFINITION_BOOLEAN:
			*((tyran_boolean*)data) = tyran_value_boolean(value);
			break;
		case NIMBUS_COMPONENT_DEFINITION_INTEGER:
			*((int*)data) = (int) tyran_value_number(value);
			break;
		//NIMBUS_COMPONENT_DEFINITION_STRING,
		case NIMBUS_COMPONENT_DEFINITION_SYMBOL:
			*((tyran_symbol*)data) = tyran_value_symbol(value);
			break;
		case NIMBUS_COMPONENT_DEFINITION_VECTOR2:
			o = tyran_value_object(value);
			nimbus_property_reader_vector2(reader, (nimbus_vector2*)data, o);
			break;
		case NIMBUS_COMPONENT_DEFINITION_VECTOR3:
			o = tyran_value_object(value);
			nimbus_property_reader_vector3(reader, (nimbus_vector3*)data, o);
			break;
		case NIMBUS_COMPONENT_DEFINITION_SIZE2I:
			o = tyran_value_object(value);
			nimbus_property_reader_size2i(reader, (nimbus_size2i*)data, o);
			break;
		case NIMBUS_COMPONENT_DEFINITION_SIZE2:
			o = tyran_value_object(value);
			nimbus_property_reader_size2(reader, (nimbus_size2*)data, o);
			break;
		case NIMBUS_COMPONENT_DEFINITION_RECT2:
			o = tyran_value_object(value);
			nimbus_property_reader_rect(reader, (nimbus_rect*)data, o);
			break;
		case NIMBUS_COMPONENT_DEFINITION_ROTATION:
			o = tyran_value_object(value);
			nimbus_property_reader_quaternion(reader, (nimbus_quaternion*)data, o);
			break;
		case NIMBUS_COMPONENT_DEFINITION_REFERENCE_INTRA:
			*((nimbus_event_intra_reference*)data) = parse_intra_reference(value);
			break;
		case NIMBUS_COMPONENT_DEFINITION_REFERENCE_RESOURCE:
			*((nimbus_event_resource_reference*)data) = parser_resource_reference(value);
			break;
	}
}

static void parse_property(nimbus_component* component, tyran_symbol symbol, const tyran_value* value)
{
	const component_definition* definition = component->component_definition;
	const nimbus_component_definition_property* property = nimbus_component_definition_property_from_type(definition, symbol);
	value_to_property(component, property, value);
}

static void iterate_component(nimbus_script_combine_parser* self, tyran_object* component_object)
{
	tyran_symbol type_symbol = lookup_type(self->mocha_api.symbol_table, component_object);
	const nimbus_component_definition* component_definition = component_definition_from_type(self, type_symbol);

	nimbus_component* component = nimbus_combine_add_component(combine, component_definition);

	tyran_property_iterator it;
	tyran_property_iterator_init_shallow(&it, combine_script_object);

	tyran_symbol symbol;
	const tyran_value* value;

	while (tyran_property_iterator_next(&it, &symbol, &value)) {
		const char* debug_key_string = tyran_symbol_table_lookup(self->mocha_api.symbol_table, &symbol);
		TYRAN_LOG("Combine: %s", debug_key_string);
		parse_property(component, symbol, tyran_value);
	}

	tyran_property_iterator_free(&it);
}

static void iterate_combine(nimbus_script_combine_parser* self, tyran_object* combine_script_object)
{
	tyran_property_iterator it;
	tyran_property_iterator_init_shallow(&it, combine_script_object);

	tyran_symbol symbol;
	const tyran_value* value;

	while (tyran_property_iterator_next(&it, &symbol, &value)) {
		const char* debug_key_string = tyran_symbol_table_lookup(self->mocha_api.symbol_table, &symbol);
		TYRAN_LOG("Combine: %s", debug_key_string);
		iterate_component();
	}

	tyran_property_iterator_free(&it);
}

static void parse_combine(nimbus_script_combine_parser* self, tyran_object* combine_script_object, nimbus_resource_id resource_id)
{

}