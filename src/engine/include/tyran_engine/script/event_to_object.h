#ifndef nimbus_event_to_object_h
#define nimbus_event_to_object_h

#include <tyran_engine/script/property_writer.h>
#include <tyranscript/tyran_symbol.h>

struct tyran_symbol_table;
struct tyran_runtime;
struct tyran_value;
struct nimbus_event_read_stream;
struct nimbus_event_definition;

typedef struct nimbus_event_to_object {
	struct tyran_runtime* runtime;
	nimbus_property_writer writer;
	tyran_symbol x_symbol;
	tyran_symbol y_symbol;
	tyran_symbol z_symbol;
	tyran_symbol width_symbol;
	tyran_symbol height_symbol;
} nimbus_event_to_object;

void nimbus_event_to_object_init(nimbus_event_to_object* self, struct tyran_symbol_table* symbol_table, struct tyran_runtime* runtime);
int nimbus_event_to_object_convert(nimbus_event_to_object* self, struct tyran_object* o, const u8t* data, struct nimbus_event_definition* e);

#endif
