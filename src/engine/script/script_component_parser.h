#ifndef nimbus_script_component_parser_h
#define nimbus_script_component_parser_h

#include <tyran_engine/resource/id.h>
#include "property_reader.h"
#include "script_property_parser.h"

#include <tyranscript/tyran_symbol.h>

struct nimbus_modules;
struct nimbus_combine;
struct tyran_object;
struct tyran_symbol_table;

typedef struct nimbus_script_component_parser {
	struct tyran_symbol_table* symbol_table;
	nimbus_script_property_parser property_parser;
} nimbus_script_component_parser;

void nimbus_script_component_parser_init(nimbus_script_component_parser* self, struct nimbus_modules* modules, struct tyran_symbol_table* symbol_table, struct nimbus_combine* combine, const struct tyran_object* combine_script_object, tyran_symbol component_name);

#endif
