#include "yaml_converter.h"
#include <yaml.h>
#include <tyranscript/tyran_log.h>
#include <tyranscript/tyran_symbol_table.h>
#include <tyranscript/tyran_symbol.h>
#include <tyran_engine/state/combine.h>
#include <tyran_engine/state/component_definition.h>
#include <tyran_engine/state/event_component_header.h>

#include <stdlib.h>

void push_scope(nimbus_yaml_converter* self)
{
	TYRAN_LOG("PUSH SCOPE");
	TYRAN_ASSERT(self->scope_index < 16, "Pushed too many scopes");
	nimbus_yaml_converter_scope* scope = &self->scopes[self->scope_index++];
	scope->component = self->last_component;
	scope->property_definition = self->property_definition;
	self->last_component = 0;
	self->property_definition = 0;

}

void pop_scope(nimbus_yaml_converter* self)
{
	TYRAN_LOG("POP SCOPE");
	TYRAN_ASSERT(self->scope_index > 0, "Popped too many scopes");
	nimbus_yaml_converter_scope* scope = &self->scopes[--self->scope_index];
	self->last_component = scope->component;
	self->property_definition = scope->property_definition;
}

static void map_start(nimbus_yaml_converter* self)
{
	struct nimbus_component* last_component = self->last_component;
	const struct nimbus_component_definition_property* last_property = self->property_definition;
	push_scope(self);

	TYRAN_LOG("map_start");
	switch (self->state) {
		case NYCS_COMBINE:
			self->state = NYCS_COMBINE_NAME;
			break;
		case NYCS_COMPONENT_FIELD_VALUE:
			if (last_property->type == NIMBUS_COMPONENT_DEFINITION_OBJECT) {
				static nimbus_component fake_component;
				fake_component.component_data = (struct nimbus_event_component_header*) ((u8t*) last_component->component_data + last_property->offset_in_struct);
				fake_component.component_definition = last_property->object_definition;
				self->last_component = &fake_component;
				TYRAN_ASSERT(self->last_component != 0, "Couldn't create sub-component");
				self->state = NYCS_COMPONENT_FIELD_NAME;
			}
			break;
		default:
			TYRAN_LOG("UNKNOWN SCOPE START!");
	}
}

static void map_end(nimbus_yaml_converter* self)
{
	TYRAN_LOG("map_end");
	pop_scope(self);
	if (self->last_component == 0) {
		self->state = NYCS_COMPONENT_NAME;
	} else {
		self->state = NYCS_COMPONENT_FIELD_NAME;
	}
}

static const nimbus_component_definition* component_definition_from_type(nimbus_yaml_converter* self, const tyran_symbol* type_name)
{
	for (int i=0; i<self->component_definitions_count; ++i) {
		nimbus_component_definition* def = &self->component_definitions[i];
		if (tyran_symbol_equal(&def->type_symbol, type_name)) {
			return def;
		}

	}
	return 0;
}

#define PROPERTY_DATA_POINTER (((uint8_t*)self->last_component->component_data) + self->property_definition->offset_in_struct)

#define MEMCPY_RAW(T, V) tyran_memcpy_type(T, (T *) PROPERTY_DATA_POINTER, V, 1);

#define MEMCPY(T, V) MEMCPY_RAW(T, &V)


static int scalar(nimbus_yaml_converter* self, const char* str)
{
	TYRAN_LOG("scalar:'%s'", str);
	switch (self->state) {
		case NYCS_COMBINE_NAME:
			TYRAN_LOG("Combine-name:'%s'", str);
			self->state = NYCS_COMPONENT_NAME;
			break;
		case NYCS_COMPONENT_NAME:
			TYRAN_LOG("Component-name:'%s'", str);
			tyran_symbol_table_add(self->symbol_table, &self->last_component_name, str);
			self->state = NYCS_COMPONENT_TYPE_FIELD;
			break;
		case NYCS_COMPONENT_TYPE_FIELD:
			if (!tyran_str_equal("type", str)) {
				TYRAN_SOFT_ERROR("Missing type for component");
				return -1;
			}
			TYRAN_LOG("type found");
			self->state = NYCS_COMPONENT_TYPE_VALUE;
			break;
		case NYCS_COMPONENT_TYPE_VALUE: {
			TYRAN_LOG("type: '%s'", &str[1]);
			tyran_symbol type_symbol;
			tyran_symbol_table_add(self->symbol_table, &type_symbol, &str[1]);
			const nimbus_component_definition* component_definition = component_definition_from_type(self, &type_symbol);
			self->state = NYCS_COMPONENT_FIELD_NAME;
			self->last_component = nimbus_combine_create_component(self->combine, self->last_component_name, component_definition);
			TYRAN_ASSERT(self->last_component != 0, "Couldn't create component");
		}
		break;
		case NYCS_COMPONENT_FIELD_NAME: {
			TYRAN_LOG("Field: '%s'", str);
			tyran_symbol field_name;
			tyran_symbol_table_add(self->symbol_table, &field_name, str);
			const nimbus_component_definition* def = self->last_component->component_definition;
			const nimbus_component_definition_property* next_property = nimbus_component_definition_property_from_name(def, field_name);
			self->state = NYCS_COMPONENT_FIELD_VALUE;
			self->property_definition = next_property;
		}
		break;
		case NYCS_COMPONENT_FIELD_VALUE: {
			TYRAN_LOG("field value: '%s'", str);
			self->state = NYCS_COMPONENT_FIELD_NAME;
			switch (self->property_definition->type) {
				case 	NIMBUS_COMPONENT_DEFINITION_FLOAT: {
					float v = atof(str);
					MEMCPY(float, v);
					break;
				}
				case NIMBUS_COMPONENT_DEFINITION_BOOLEAN: {
					tyran_boolean b = tyran_str_equal(str, "true");
					MEMCPY(tyran_boolean, b);
					break;
				}
				case NIMBUS_COMPONENT_DEFINITION_INTEGER: {
					int i;
					if (str[0] == '0' && str[1] == 'x') {
						i = (int) strtol(&str[2], 0, 16);
					} else {
						i = atoi(str);
					}
					MEMCPY(int, i);
					break;
				}
				case NIMBUS_COMPONENT_DEFINITION_STRING:
					break;

				case NIMBUS_COMPONENT_DEFINITION_SYMBOL: {
					break;
				}
				case NIMBUS_COMPONENT_DEFINITION_REFERENCE_INTRA: {
					const char* name = str;
					tyran_symbol intra_symbol;
					tyran_symbol_table_add(self->symbol_table, &intra_symbol, name);
					nimbus_component* component = nimbus_combine_component_from_name(self->combine, intra_symbol);

					MEMCPY(void*, component->component_data);
					break;
				}
				case NIMBUS_COMPONENT_DEFINITION_REFERENCE_RESOURCE: {
					const char* name = str;
					char temp_name[256];
					const char* component_name = 0;
					const char* separator = tyran_str_chr(name, '.');
					if (separator) {
						component_name = separator + 1;
						tyran_strncpy(temp_name, 256, str, (separator - str));
						name = temp_name;
					}
					nimbus_resource_id resource_symbol = nimbus_resource_id_from_string(name);

					tyran_symbol component_symbol;
					component_symbol.hash = 0;
					if (component_name) {
						tyran_symbol_table_add(self->symbol_table, &component_symbol, component_name);
					}

					tyran_symbol property_symbol;
					property_symbol.hash = 0;

					nimbus_combine_component_add_extra_reference(self->last_component, PROPERTY_DATA_POINTER, resource_symbol, component_symbol, property_symbol);
					break;
				}
			}
		}

	}

	return 0;
}



void nimbus_yaml_converter_init(nimbus_yaml_converter* self, tyran_symbol_table* symbol_table, struct nimbus_component_definition* component_definitions, int component_definitions_count, nimbus_combine* combine)
{
	self->state = NYCS_COMBINE;
	self->combine = combine;
	self->symbol_table = symbol_table;
	self->component_definitions = component_definitions;
	self->component_definitions_count = component_definitions_count;
	self->scope_index = 0;

}

int nimbus_yaml_converter_parse(nimbus_yaml_converter* self, const char* input, size_t size)
{
	yaml_parser_t parser;
	yaml_event_t  event;

	if(!yaml_parser_initialize(&parser)) {
		TYRAN_ERROR("Couldn't initialize yaml parser");
		return 0;
	}

	yaml_parser_set_input_string(&parser, (const unsigned char*)input, size);

	do {
		if (!yaml_parser_parse(&parser, &event)) {
			TYRAN_ERROR("Parser error %d", parser.error);
			return 0;
		}

		switch(event.type) {
			case YAML_NO_EVENT:
				TYRAN_LOG("No event!");
				break;
			case YAML_STREAM_START_EVENT:
				TYRAN_LOG("STREAM START");
				break;
			case YAML_STREAM_END_EVENT:
				TYRAN_LOG("STREAM END");
				break;
			case YAML_DOCUMENT_START_EVENT:
				TYRAN_LOG("<b>Start Document</b>");
				break;
			case YAML_DOCUMENT_END_EVENT:
				TYRAN_LOG("<b>End Document</b>");
				break;
			case YAML_SEQUENCE_START_EVENT:
				TYRAN_LOG("<b>Start Sequence</b>");
				break;
			case YAML_SEQUENCE_END_EVENT:
				TYRAN_LOG("<b>End Sequence</b>");
				break;
			case YAML_MAPPING_START_EVENT:
				map_start(self);
				break;
			case YAML_MAPPING_END_EVENT:
				map_end(self);
				break;
			case YAML_ALIAS_EVENT:
				TYRAN_LOG("Got alias (anchor %s)", event.data.alias.anchor);
				break;
			case YAML_SCALAR_EVENT:
				scalar(self, (const char*)event.data.scalar.value);
				break;
		}
		if(event.type != YAML_STREAM_END_EVENT) {
			yaml_event_delete(&event);
		}
	} while(event.type != YAML_STREAM_END_EVENT);

	yaml_event_delete(&event);

	yaml_parser_delete(&parser);

	return 0;
}
