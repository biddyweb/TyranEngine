#ifndef nimbus_event_definition_h
#define nimbus_event_definition_h

#include <tyranscript/tyran_types.h>
#include <tyranscript/tyran_symbol.h>

struct tyran_symbol_table;

typedef enum nimbus_event_definition_type {
	NIMBUS_EVENT_DEFINITION_FLOAT,
	NIMBUS_EVENT_DEFINITION_INTEGER,
	NIMBUS_EVENT_DEFINITION_STRING,
	NIMBUS_EVENT_DEFINITION_OBJECT,
	NIMBUS_EVENT_DEFINITION_VECTOR2,
	NIMBUS_EVENT_DEFINITION_VECTOR3,
	NIMBUS_EVENT_DEFINITION_SIZE2I,
	NIMBUS_EVENT_DEFINITION_SIZE2,
	NIMBUS_EVENT_DEFINITION_RECT2,
	NIMBUS_EVENT_DEFINITION_ROTATION,
} nimbus_event_definition_type;

typedef struct nimbus_event_definition_property {
	nimbus_event_definition_type type;
	tyran_symbol symbol;
} nimbus_event_definition_property;

typedef struct nimbus_event_definition {
	nimbus_event_definition_property properties[16];
	int properties_count;
	tyran_boolean has_index;
	u8t event_type_id;
	const char* name;
	tyran_symbol type_symbol;
	struct tyran_symbol_table* symbol_table;
	tyran_boolean is_module_to_script;
	tyran_boolean should_instantiate;
} nimbus_event_definition;

void nimbus_event_definition_init(nimbus_event_definition* self, struct tyran_symbol_table* symbol_table, const char* name, u8t event_type_id);
void nimbus_event_definition_add_property(nimbus_event_definition* self, const char* name, nimbus_event_definition_type type);

#endif
