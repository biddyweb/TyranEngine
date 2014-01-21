#include <tyran_engine/event/key_changed.h>

const u8t NIMBUS_EVENT_KEY_CHANGED_ID = 11;

void nimbus_key_changed_send(struct nimbus_event_write_stream* stream, u32t virtual_key_id, tyran_boolean key_is_down)
{
	nimbus_key_changed e;
	e.virtual_key_id = virtual_key_id;
	e.key_is_down = key_is_down;

	nimbus_event_stream_write_event(stream, NIMBUS_EVENT_KEY_CHANGED_ID, e);
}
