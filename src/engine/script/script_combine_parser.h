#ifndef nimbus_script_combine_parser_h
#define nimbus_script_combine_parser_h

#include <tyran_engine/resource/id.h>
#include "property_reader.h"
#include "script_component_parser.h"

struct nimbus_modules;
struct nimbus_combine;
struct tyran_object;
struct tyran_symbol_table;
struct nimbus_resource_cache;

typedef struct nimbus_script_combine_parser {
	struct tyran_symbol_table* symbol_table;
	nimbus_script_component_parser component_parser;
	struct nimbus_resource_cache* resource_cache;
} nimbus_script_combine_parser;

void nimbus_script_combine_parser_init(nimbus_script_combine_parser* self, struct nimbus_modules* modules, struct tyran_symbol_table* symbol_table, struct nimbus_resource_cache* resource_cache, struct nimbus_combine* combine, const struct tyran_object* combine_script_object, nimbus_resource_id resource_id);

#endif
