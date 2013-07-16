#ifndef nimbus_modules_h
#define nimbus_modules_h

#include <tyran_core/update/update.h>
#include "nimbus_module.h"
#include <tyran_engine/event_definition/event_definition.h>

struct nimbus_event_write_stream;
struct tyran_object;
struct nimbus_property_reader;


typedef void (*nimbus_event_converter_func)(struct nimbus_event_write_stream* stream, struct nimbus_property_reader* reader, struct tyran_object* o);

typedef struct nimbus_modules {
	nimbus_module modules[16];
	int modules_count;
	int modules_max_count;
	nimbus_event_definition* event_definitions;
	int event_definitions_count;
	int event_definitions_max_count;
} nimbus_modules;

void nimbus_modules_init(nimbus_modules* self, struct tyran_memory* memory);
void nimbus_modules_add(nimbus_modules* self, const char* name, size_t octet_size, nimbus_module_init_function func, size_t update_offset);
struct nimbus_event_definition* nimbus_modules_add_event(nimbus_modules* self, const char* name, u8t event_type_id);

#endif
