#ifndef nimbus_touch_changed_h
#define nimbus_touch_changed_h

#include <tyran_engine/type/vector2.h>
#include <tyran_core/event/event_stream.h>

extern const nimbus_event_type_id NIMBUS_EVENT_TOUCH_BEGAN_ID;
extern const nimbus_event_type_id NIMBUS_EVENT_TOUCH_MOVED_ID;
extern const nimbus_event_type_id NIMBUS_EVENT_TOUCH_ENDED_ID;
extern const nimbus_event_type_id NIMBUS_EVENT_TOUCH_STATIONARY_ID;

struct nimbus_event_write_stream;

typedef enum nimbus_touch_phase {
	nimbus_touch_phase_began,
	nimbus_touch_phase_moved,
	nimbus_touch_phase_ended,
	nimbus_touch_phase_cancelled
} nimbus_touch_phase;

typedef struct nimbus_touch_changed {
	nimbus_vector2 position;
	nimbus_touch_phase phase;
} nimbus_touch_changed;


#endif
