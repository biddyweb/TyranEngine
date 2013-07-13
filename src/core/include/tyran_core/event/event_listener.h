#ifndef nimbus_event_listener_h
#define nimbus_event_listener_h

#include <tyran_core/event/event_stream.h>

struct tyran_memory;

typedef void (*nimbus_event_read)(void* self, struct nimbus_event_read_stream* stream);

typedef struct nimbus_event_listener_function {
	nimbus_event_read event_reader;
	nimbus_event_type_id id;
} nimbus_event_listener_function;

typedef struct nimbus_event_listener {
	nimbus_event_listener_function functions[100];
	int function_count;
	nimbus_event_read listen_to_all;
	struct tyran_memory* memory;
	void* other_self;
} nimbus_event_listener;

void nimbus_event_listener_init(nimbus_event_listener* self, void* other_self);
void nimbus_event_listener_listen(nimbus_event_listener* self, nimbus_event_type_id id, nimbus_event_read reader);

void nimbus_event_process(nimbus_event_listener* self, struct nimbus_event_read_stream* read_stream);
#endif
