#include <tyran_engine/event/unspawn_event.h>
#include <tyran_core/event/event_stream.h>

void nimbus_event_unspawn_send(struct nimbus_event_write_stream* stream, u8t event_type_id, int instance_index)
{
	nimbus_event_unspawn event;
	event.instance_index = instance_index;
	nimbus_event_stream_write_event(stream, event_type_id, event);
}
