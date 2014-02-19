#include "sound_wave_updated.h"
#include <tyran_core/event/event_stream.h>
#include "../sound_wave.h"

const u8t NIMBUS_EVENT_SCRIPT_OBJECT_UPDATED_ID = 101;

void nimbus_script_object_updated_send(struct nimbus_event_write_stream* stream, nimbus_resource_id id, const tyran_object* object)
{
	nimbus_script_object_updated event;
	event.resource_id = id;
	event.object = object;

	nimbus_event_stream_write_event(stream, NIMBUS_EVENT_SCRIPT_OBJECT_UPDATED_ID, event);
}
