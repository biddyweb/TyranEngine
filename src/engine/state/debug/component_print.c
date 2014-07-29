void print_property_value(nimbus_component_definition_property_type type, const u8t* data)
{
	switch (type) {
		case NIMBUS_COMPONENT_DEFINITION_FLOAT: {
			const float* v = (const float*) data;
			TYRAN_OUTPUT("f: %f", *v);
		}
		break;
		case NIMBUS_COMPONENT_DEFINITION_BOOLEAN: {
			const tyran_boolean* v = (const tyran_boolean*) data;
			TYRAN_OUTPUT("b: %d", *v);
		}
		break;
		case NIMBUS_COMPONENT_DEFINITION_INTEGER: {
			const int* v = (const int *) data;
			TYRAN_OUTPUT("i: %d", *v);
		}
		break;
		case NIMBUS_COMPONENT_DEFINITION_STRING: {
			TYRAN_OUTPUT("Not implemented yet");
		}
		break;
		case NIMBUS_COMPONENT_DEFINITION_SYMBOL: {
			const tyran_symbol* v = (const tyran_symbol*) data;
			TYRAN_OUTPUT("symbol: %d", v->hash);
		}
		case NIMBUS_COMPONENT_DEFINITION_OBJECT: {
			TYRAN_OUTPUT("Object: Not implemented yet");
		}
		break;
		case NIMBUS_COMPONENT_DEFINITION_REFERENCE_INTRA: {
			TYRAN_OUTPUT("Intra: Not implemented yet");
		}
		break;
		case NIMBUS_COMPONENT_DEFINITION_REFERENCE_RESOURCE: {
			TYRAN_OUTPUT("Resource: Not implemented yet");
		}
		break;
	}
}

void print_extra_reference(nimbus_extra_reference* ref)
{
	TYRAN_OUTPUT("extra:");
	TYRAN_OUTPUT("resource id:%d", ref->resource_id);
	TYRAN_OUTPUT("component_name:%d", ref->property_name);
}

void print_extra_references(const nimbus_component* component)
{
	TYRAN_OUTPUT("Extra references:");
	for (int i=0; i<component->extra_references_count; ++i) {
		nimbus_extra_reference* ref = &component->extra_references[i];
		print_extra_reference(ref);
	}
}

void print_property(const nimbus_component_definition_property* property, const u8t* data)
{
	print_property_value(type, data);
}

void print_component_data_with_definition(nimbus_event_component_header* data, nimbus_component_definition* definition)
{
	for (int i=0; i<definition->properties_count; ++i) {
		const nimbus_component_definition_property* property = &definition->properties[i];
		const u8t* raw_property = (const u8t*) data + property->offset_in_struct;
		print_property(property, raw_property);
	}
}

void print_component(nimbus_component* component)
{
	print_component_data_with_definition(component->component_data, component->component_definition);
	print_extra_references(component);
}
