#ifndef nimbus_object_to_event_h
#define nimbus_object_to_event_h

#include <tyran_engine/script/property_reader.h>

struct nimbus_event_write_stream;
struct nimbus_event_definition;


typedef struct nimbus_object_to_event {
	nimbus_property_reader property_reader;
	u8t* temp_buf;
	int temp_buf_max_size;
	struct tyran_symbol_table* symbol_table;
} nimbus_object_to_event;

void nimbus_object_to_event_init(nimbus_object_to_event* self, struct tyran_memory* memory, struct tyran_symbol_table* symbol_table);
void nimbus_object_to_event_free(nimbus_object_to_event* self);
void nimbus_object_to_event_convert(nimbus_object_to_event* self, struct nimbus_event_write_stream* stream, struct tyran_object* o, struct nimbus_event_definition* e);

#endif
