#ifndef nimbus_script_state_parser_h
#define nimbus_script_state_parser_h

#include <tyran_engine/resource/id.h>
#include "property_reader.h"

struct nimbus_modules;
struct nimbus_combine;
struct tyran_object;
struct tyran_symbol_table;
struct nimbus_state;

typedef struct nimbus_script_state_parser {
	nimbus_property_reader property_reader;
	struct tyran_symbol_table* symbol_table;
	nimbus_resource_id resource_id;
	struct nimbus_modules* modules;
	struct nimbus_state* state;
} nimbus_script_state_parser;

void nimbus_script_state_parser_init(nimbus_script_state_parser* self, struct nimbus_modules* modules, struct tyran_symbol_table* symbol_table, struct nimbus_state* state, const struct tyran_object* state_script_object, nimbus_resource_id resource_id);

#endif