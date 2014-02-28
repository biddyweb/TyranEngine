#ifndef nimbus_component_definition_h
#define nimbus_component_definition_h

struct tyran_symbol_table;

#include <tyranscript/tyran_types.h>
#include <tyranscript/tyran_symbol.h>

#include <tyran_engine/resource/type_id.h>

typedef enum nimbus_component_definition_property_type {
	NIMBUS_COMPONENT_DEFINITION_FLOAT,
	NIMBUS_COMPONENT_DEFINITION_BOOLEAN,
	NIMBUS_COMPONENT_DEFINITION_INTEGER,
	NIMBUS_COMPONENT_DEFINITION_STRING,
	NIMBUS_COMPONENT_DEFINITION_SYMBOL,
	NIMBUS_COMPONENT_DEFINITION_VECTOR2,
	NIMBUS_COMPONENT_DEFINITION_VECTOR3,
	NIMBUS_COMPONENT_DEFINITION_SIZE2I,
	NIMBUS_COMPONENT_DEFINITION_SIZE2,
	NIMBUS_COMPONENT_DEFINITION_RECT2,
	NIMBUS_COMPONENT_DEFINITION_ROTATION,
	NIMBUS_COMPONENT_DEFINITION_REFERENCE_INTRA,
	NIMBUS_COMPONENT_DEFINITION_REFERENCE_RESOURCE
} nimbus_component_definition_property_type;

typedef struct nimbus_component_definition_property {
	nimbus_component_definition_property_type type;
	tyran_symbol symbol;
	size_t offset_in_struct;
	nimbus_resource_type_id reference_resource_type_id;
} nimbus_component_definition_property;

typedef struct nimbus_component_definition {
	nimbus_component_definition_property properties[16];
	int properties_count;
	u8t event_type_id;
	const char* debug_name;
	tyran_symbol type_symbol;
	size_t struct_size;
	struct tyran_symbol_table* symbol_table;
} nimbus_component_definition;

void nimbus_component_definition_init(nimbus_component_definition* self, struct tyran_symbol_table* symbol_table, const char* debug_name, u8t event_type_id, size_t struct_size);
void nimbus_component_definition_add_property(nimbus_component_definition* self, const char* name, nimbus_component_definition_property_type type, size_t offset);
const nimbus_component_definition_property* nimbus_component_definition_property_from_type(const nimbus_component_definition* self, tyran_symbol symbol);

#endif
