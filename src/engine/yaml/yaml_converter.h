#ifndef yaml_converter_h
#define yaml_converter_h

#include <tyranscript/tyran_symbol.h>

struct tyran_symbol_table;
struct nimbus_combine;
struct nimbus_component_definition;
struct nimbus_component_definition_property;
struct nimbus_component;

#include <stddef.h>

typedef enum nimbus_yaml_convert_state {
	NYCS_COMBINE,
	NYCS_COMBINE_NAME,
	NYCS_COMPONENT_NAME,
	NYCS_COMPONENT_TYPE_FIELD,
	NYCS_COMPONENT_TYPE_VALUE,
	NYCS_COMPONENT_FIELD_NAME,
	NYCS_COMPONENT_FIELD_VALUE
} nimbus_yaml_convert_state;

typedef struct nimbus_yaml_converter_scope {
	const struct nimbus_component_definition_property* property_definition;
	struct nimbus_component* component;
} nimbus_yaml_converter_scope;

typedef struct nimbus_yaml_converter {
	nimbus_yaml_convert_state state;
	tyran_symbol last_component_name;
	struct tyran_symbol_table* symbol_table;
	struct nimbus_combine* combine;
	const struct nimbus_component_definition_property* property_definition;
	struct nimbus_component_definition* component_definitions;
	int component_definitions_count;
	struct nimbus_component* last_component;

	nimbus_yaml_converter_scope scopes[16];
	int scope_index;
} nimbus_yaml_converter;

void nimbus_yaml_converter_init(nimbus_yaml_converter* self, struct tyran_symbol_table* symbol_table,  struct nimbus_component_definition* component_definitions, int component_definitions_count, struct nimbus_combine* combine);
int nimbus_yaml_converter_parse(nimbus_yaml_converter* self, const char* input, size_t size);

#endif
