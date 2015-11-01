#include <tyran_engine/event/resource_load.h>
#include <tyran_core/event/event_stream.h>
#include <tyranscript/tyran_log.h>

const u8t NIMBUS_EVENT_RESOURCE_LOAD = 1;

void nimbus_resource_load_send(nimbus_event_write_stream* stream, nimbus_resource_id id, nimbus_resource_type_id resource_type_id)
{
	nimbus_resource_load event;
	event.resource_id = id;
	event.resource_type_id = resource_type_id;
	TYRAN_LOG("Load id:%d resource_name:'%s' type:%d", id, nimbus_resource_id_debug_name(id), resource_type_id);
	nimbus_event_stream_write_event(stream, NIMBUS_EVENT_RESOURCE_LOAD, event);
}
