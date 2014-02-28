#include "script_property_parser.h"

#include <tyranscript/tyran_value.h>
#include <tyran_engine/event/intra_reference.h>
#include <tyran_engine/resource/type_id.h>
#include <tyran_engine/resource/resource.h>
#include <tyran_engine/state/component.h>
#include <tyranscript/tyran_string.h>
#include "property_reader.h"
#include "component_definition.h"
#include "../resource/resource_cache.h"

nimbus_event_intra_reference parse_intra_reference(const tyran_value* value)
{
	nimbus_event_intra_reference intra;

	return intra;
}

const nimbus_resource* parse_resource_reference(nimbus_resource_cache* cache, const tyran_value* value, nimbus_resource_type_id type_id)
{
	const tyran_string* str = tyran_value_string(value);
	char resource_path[256];
	tyran_string_to_c_str(resource_path, 256, str);

	const char* name = resource_path;

	if (resource_path[0] == '@') {
		name = &resource_path[1];
	}
	nimbus_resource_id resource_id = nimbus_resource_id_from_string(name);

	nimbus_resource_cache_find_or_create(cache, resource_id, type_id);
	const nimbus_resource* resource = 0;
	
	return resource;
}

static void value_to_property(nimbus_resource_cache* cache, nimbus_component* component, const nimbus_component_definition_property* property, const tyran_value* value, nimbus_property_reader* reader)
{
	void* data = (void*) ((u8t*)component->component_data + property->offset_in_struct);
	const struct tyran_object* o;
	switch (property->type) {
		case NIMBUS_COMPONENT_DEFINITION_FLOAT:
			*((float*)data) = tyran_value_number(value);
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
			*((const nimbus_resource**)data) = parse_resource_reference(cache, value, property->reference_resource_type_id);
			break;
		default:
			TYRAN_ERROR("Not handled type:%d", property->type);
	}
}

static void parse_property(nimbus_resource_cache* cache, nimbus_component* component, nimbus_property_reader* reader, tyran_symbol symbol, const tyran_value* value)
{
	const nimbus_component_definition* definition = component->component_definition;
	const nimbus_component_definition_property* property = nimbus_component_definition_property_from_type(definition, symbol);
	value_to_property(cache, component, property, value, reader);
}

void script_property_parser_init(nimbus_script_property_parser* self, struct nimbus_resource_cache* cache, struct tyran_symbol_table* symbol_table)
{
	self->resource_cache = cache;
	nimbus_property_reader_init(&self->property_reader, symbol_table);
}

void nimbus_script_property_parser_parse_property(nimbus_script_property_parser* self, nimbus_component* component, tyran_symbol symbol, const tyran_value* value)
{
	parse_property(self->resource_cache, component, &self->property_reader, symbol, value);
}
