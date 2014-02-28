#ifndef nimbus_script_property_parser_h
#define nimbus_script_property_parser_h

#include <tyranscript/tyran_symbol.h>

#include "property_reader.h"

struct nimbus_component;
struct tyran_value;
struct tyran_symbol_table;

typedef struct nimbus_script_property_parser {
	nimbus_property_reader property_reader;
} nimbus_script_property_parser;

void script_property_parser_init(nimbus_script_property_parser* self, struct tyran_symbol_table* symbol_table);
void nimbus_script_property_parser_parse_property(nimbus_script_property_parser* self, struct nimbus_component* component, tyran_symbol symbol, const struct tyran_value* value);

#endif
