#include "resource_load_state.h"
#include "../../core/src/base/event/nimbus_event_stream.h"

const u8t NIMBUS_EVENT_RESOURCE_LOAD_STATE = 14;


void nimbus_resource_load_state_send(nimbus_event_write_stream* stream, nimbus_resource_id id)
{
	nimbus_resource_load_state event;
	event.resource_id = id;
	nimbus_event_stream_write_event(stream, NIMBUS_EVENT_RESOURCE_LOAD_STATE, event);
}
