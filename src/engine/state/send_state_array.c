#include "send_state_array.h"
#include "component_data_array.h"

#include <tyran_engine/event/state_component_event.h>
#include <tyran_core/event/event_stream.h>
#include <tyran_engine/state/component_definition.h>

void nimbus_send_state_array(const nimbus_component_array* array, struct nimbus_event_write_stream* stream)
{
	const struct nimbus_component_definition* definition = array->definition;
	nimbus_state_component_event event;
	event.data = array->data;
	event.count = array->max_count;
	nimbus_event_stream_write_event(stream, definition->event_type_id, event);
}
