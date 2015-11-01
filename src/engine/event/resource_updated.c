#include <tyran_engine/event/resource_updated.h>
#include <tyran_core/event/event_stream.h>
#include <tyranscript/tyran_log.h>

const u8t NIMBUS_EVENT_RESOURCE_UPDATED = 2;

void nimbus_resource_updated_send(nimbus_event_write_stream* out_event_stream, nimbus_resource_id resource_id,
								  nimbus_resource_type_id resource_type_id, const void* data, int payload_size)
{
	nimbus_resource_updated updated;
	updated.resource_id = resource_id;
	updated.resource_type_id = resource_type_id;
	updated.payload_size = payload_size;

	nimbus_event_stream_write_event_header(out_event_stream, NIMBUS_EVENT_RESOURCE_UPDATED);
	nimbus_event_stream_write_type(out_event_stream, updated);
	nimbus_event_stream_write_octets(out_event_stream, data, payload_size);
	nimbus_event_stream_write_event_end(out_event_stream);
}
