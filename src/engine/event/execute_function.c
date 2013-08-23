#include "execute_function.h"
#include <tyran_core/event/event_stream.h>

const u8t NIMBUS_EVENT_EXECUTE_FUNCTION = 15;

void nimbus_event_execute_function_send(nimbus_event_write_stream* stream, nimbus_combine_instance_id instance_id, tyran_symbol function_symbol)
{
	nimbus_event_execute_function event;
	event.instance_id = instance_id;
	event.function_symbol = function_symbol;
	nimbus_event_stream_write_event(stream, NIMBUS_EVENT_EXECUTE_FUNCTION, event);
}
