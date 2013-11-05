#ifndef nimbus_touch_changed_h
#define nimbus_touch_changed_h

#include <tyran_engine/type/vector2.h>
#include <tyran_core/event/event_stream.h>

extern const nimbus_event_type_id NIMBUS_EVENT_TOUCH_BEGAN_ID;
extern const nimbus_event_type_id NIMBUS_EVENT_TOUCH_MOVED_ID;
extern const nimbus_event_type_id NIMBUS_EVENT_TOUCH_ENDED_ID;
extern const nimbus_event_type_id NIMBUS_EVENT_TOUCH_STATIONARY_ID;
extern const nimbus_event_type_id NIMBUS_EVENT_TOUCH_ZOOM_ID;
extern const nimbus_event_type_id NIMBUS_EVENT_TOUCH_HOVER_ID;

struct nimbus_event_write_stream;


typedef struct nimbus_touch_changed {
	nimbus_vector2 position;
	int finger_id;
} nimbus_touch_changed;

void nimbus_touch_changed_send(struct nimbus_event_write_stream* stream, u8t event_type_id, nimbus_vector2 position);

#endif
