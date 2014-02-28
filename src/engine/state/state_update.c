#include "state_update.h"
#include "send_state_array.h"
#include "component_data_array.h"
#include "component_data_arrays.h"

void nimbus_state_update_send(nimbus_component_arrays* arrays, struct nimbus_event_write_stream* stream)
{
	for (int i = 0; i < arrays->tracks_count; ++i) {
		const nimbus_component_array* array = &arrays->tracks[i];
		nimbus_send_state_array(array, stream);
	}
}
