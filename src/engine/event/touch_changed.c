#include <tyran_engine/event/touch_changed.h>

const u8t NIMBUS_EVENT_TOUCH_BEGAN_ID = 3;
const u8t NIMBUS_EVENT_TOUCH_MOVED_ID = 4;
const u8t NIMBUS_EVENT_TOUCH_ENDED_ID = 5;
const u8t NIMBUS_EVENT_TOUCH_STATIONARY_ID = 6;
const u8t NIMBUS_EVENT_TOUCH_ZOOM_ID = 7;
const u8t NIMBUS_EVENT_TOUCH_HOVER_ID = 8;

void nimbus_touch_changed_send(struct nimbus_event_write_stream* stream, u8t event_type_id, u8t button_id,
							   nimbus_vector2 position)
{
	nimbus_touch_changed changed;
	changed.position.x = position.x;
	changed.position.y = position.y;
	changed.finger_id = button_id;

	nimbus_event_stream_write_event(stream, event_type_id, changed);
}
