#ifndef nimbus_modules_h
#define nimbus_modules_h

#include <tyran_core/update/update.h>
#include <tyran_engine/module/nimbus_module.h>
#include <tyran_engine/event_definition/event_definition.h>

struct nimbus_event_write_stream;
struct tyran_object;
struct nimbus_property_reader;
struct tyran_symbol_table;

typedef void (*nimbus_event_converter_func)(struct nimbus_event_write_stream* stream, struct nimbus_property_reader* reader, struct tyran_object* o);

typedef struct nimbus_modules {
	nimbus_module modules[16];
	int modules_count;
	int modules_max_count;
	nimbus_event_definition* event_definitions;
	int event_definitions_count;
	int event_definitions_max_count;
	struct tyran_symbol_table* symbol_table;
} nimbus_modules;

void nimbus_modules_init(nimbus_modules* self, struct tyran_memory* memory, struct tyran_symbol_table* symbol_table);
void nimbus_modules_add(nimbus_modules* self, const char* name, size_t octet_size, nimbus_module_init_function func, size_t update_offset);
void nimbus_modules_add_affinity(nimbus_modules* self, const char* name, size_t octet_size, nimbus_module_init_function func, size_t update_offset, int affinity);

struct nimbus_event_definition* nimbus_modules_add_event(nimbus_modules* self, const char* name, u8t event_type_id, u8t unspawn_event_type_id);

#endif
