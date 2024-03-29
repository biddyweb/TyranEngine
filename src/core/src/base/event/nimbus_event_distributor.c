#include <tyranscript/tyran_memory.h>
#include <tyranscript/tyran_log.h>

#include "nimbus_event_distributor.h"
#include <tyran_core/update/update.h>

void nimbus_event_distributor_init(nimbus_event_distributor* self, const struct tyran_symbol_table* table, tyran_memory* memory)
{
	self->event_buffer_size = 11 * 8 * 1024 * 1024;
	self->event_buffer = TYRAN_MEMORY_CALLOC(memory, self->event_buffer_size, "event_buffer");
	self->symbol_table = table;
}

void nimbus_event_distributor_set_buffer_for_objects_to_parse(nimbus_event_distributor* self, nimbus_update** objects,
															  int object_count)
{
	for (int i = 0; i < object_count; ++i) {
		nimbus_update* o = objects[i];
		nimbus_event_stream_read_init(&o->event_read_stream, self->symbol_table, self->event_buffer,
									  self->event_buffer_used_octet_size);
	}
}

void nimbus_event_distributor_write_events_to_buffer(nimbus_event_distributor* self, nimbus_update** objects, int object_count)
{
	u8t* p = self->event_buffer;

	for (int i = 0; i < object_count; ++i) {
		nimbus_update* o = objects[i];

		int events_octet_size = nimbus_event_write_stream_length(&o->event_write_stream);
		if (events_octet_size != 0) {
			const int alignment = 4;
			if (((tyran_pointer_to_number) p % alignment) != 0) {
				int skip = alignment - ((tyran_pointer_to_number) p % alignment);
				p += skip;
			}
			const u8t* events = o->event_write_stream.buffer;
			TYRAN_ASSERT((p - self->event_buffer) + events_octet_size < self->event_buffer_size, "Out of event buffer memory");

			tyran_memcpy_type(u8t, p, events, events_octet_size);
			p += events_octet_size;
		} else {
		}

		nimbus_event_write_stream_clear(&o->event_write_stream);
	}

	self->event_buffer_used_octet_size = (int) (p - self->event_buffer);
}

void nimbus_event_distributor_distribute_events(nimbus_event_distributor* self, nimbus_update** objects, int object_count)
{
	nimbus_event_distributor_write_events_to_buffer(self, objects, object_count);
	nimbus_event_distributor_set_buffer_for_objects_to_parse(self, objects, object_count);
}
