#include <tyran_engine/event/resource_load.h>
#include <tyran_core/event/event_stream.h>

const u8t NIMBUS_EVENT_RESOURCE_LOAD = 1;


void nimbus_resource_load_send(nimbus_event_write_stream* stream, nimbus_resource_id id)
{
	nimbus_resource_load event;
	event.resource_id = id;
	nimbus_event_stream_write_event(stream, NIMBUS_EVENT_RESOURCE_LOAD, event);
}
