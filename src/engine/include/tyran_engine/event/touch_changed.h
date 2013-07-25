#ifndef nimbus_touch_changed_h
#define nimbus_touch_changed_h

#include <tyran_engine/type/vector2.h>

extern const u8t NIMBUS_EVENT_TOUCH_CHANGED;

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
