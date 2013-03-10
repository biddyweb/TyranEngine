#include <tyranscript/tyran_memory.h>
#include <tyranscript/tyran_log.h>

#include "nimbus_event_distributor.h"
#include "../update/nimbus_update.h"

nimbus_event_distributor* nimbus_event_distributor_new(tyran_memory* memory)
{
	nimbus_event_distributor* self = TYRAN_MEMORY_CALLOC_TYPE(memory, nimbus_event_distributor);
	self->event_buffer_size = 8 * 1024 * 1024;
	self->event_buffer = TYRAN_MEMORY_CALLOC(memory, self->event_buffer_size, "event_buffer");

	return self;
}


void nimbus_event_distributor_set_buffer_for_objects_to_parse(nimbus_event_distributor* self, nimbus_update* objects, int object_count)
{
	for (int i=0; i<object_count; ++i) {
		nimbus_update* o = &objects[i];
		o->event_buffer = self->event_buffer;
		o->event_buffer_size = self->event_buffer_used_octet_size;
	}
}

void nimbus_event_distributor_write_events_to_buffer(nimbus_event_distributor* self, nimbus_update* objects, int object_count)
{
	u8t* p = self->event_buffer;

	for (int i=0; i<object_count; ++i) {
		nimbus_update* o = &objects[i];

		int events_octet_size = o->outgoing_event_count;
		if (events_octet_size != 0) {
			const u8t* events = o->outgoing_events;
			// TORNADO_LOG("** Distributing events of octet size:" << events_octet_size);
			TYRAN_ASSERT((p - self->event_buffer) + events_octet_size < self->event_buffer_size, "Out of event buffer memory");

			tyran_memcpy_type(u8t, p, events, events_octet_size);
			p += events_octet_size;
		}

		o->outgoing_event_count = 0;
	}

	self->event_buffer_used_octet_size = (p - self->event_buffer);
}

void nimbus_event_distributor_distribute_events(nimbus_event_distributor* self, nimbus_update* objects, int object_count)
{
	nimbus_event_distributor_write_events_to_buffer(self, objects, object_count);
	nimbus_event_distributor_set_buffer_for_objects_to_parse(self, objects, object_count);
}
