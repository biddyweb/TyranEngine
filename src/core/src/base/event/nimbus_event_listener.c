#include "nimbus_event_listener.h"
#include "nimbus_event_stream.h"
#include <tyranscript/tyran_memory.h>

nimbus_event_listener_function* nimbus_event_function_from_event_id(nimbus_event_listener* self, nimbus_event_type_id id)
{
	for (int i=0; i<self->function_count; ++i) {
		nimbus_event_listener_function* func = &self->functions[i];
		if (func->id == id) {
			return func;
		}
	}

	return 0;
}

void nimbus_event_process(nimbus_event_listener* self, const u8t* raw_event_pointer, size_t octet_size)
{
	const u8t* start_event_pointer;
	const u8t* end_pointer = raw_event_pointer + octet_size;
	const int alignment = 8;

	for (const u8t* pointer = raw_event_pointer; pointer < end_pointer; ) {
		nimbus_event_type_id id = *((nimbus_event_type_id*) pointer);
		pointer += sizeof(nimbus_event_type_id);

		u32t event_octet_size = *(u32t*) pointer;
		pointer += sizeof(u32t);

		start_event_pointer = pointer;

		pointer += event_octet_size;

		if (((tyran_pointer_to_number)pointer % alignment) != 0) {
			pointer += alignment - ((tyran_pointer_to_number)pointer % alignment);
		}

		nimbus_event_listener_function* func = nimbus_event_function_from_event_id(self, id);

		if (func) {
			nimbus_event_read_stream stream;
			stream.pointer = start_event_pointer;
			func->event_reader(func, &stream);
		}

		if (self->listen_to_all) {
			nimbus_event_read_stream stream;
			stream.pointer = start_event_pointer;
			stream.event_type_id = id;
			self->listen_to_all(func, &stream);
		}
	}
}


nimbus_event_listener* nimbus_event_listener_new(tyran_memory* memory)
{
	nimbus_event_listener* self = TYRAN_MEMORY_CALLOC_TYPE(memory, nimbus_event_listener);
	self->memory = memory;

	return self;
}

void nimbus_event_listener_listen(nimbus_event_listener* self, nimbus_event_type_id id, nimbus_event_read reader)
{
	self->functions[self->function_count].event_reader = reader;
	self->functions[self->function_count].id = id;
	self->function_count++;
}
