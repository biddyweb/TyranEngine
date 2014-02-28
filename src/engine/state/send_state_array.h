#ifndef nimbus_send_state_array_h
#define nimbus_send_state_array_h

struct nimbus_component_array;
struct nimbus_event_write_stream;

void nimbus_send_state_array(const struct nimbus_component_array* array, struct nimbus_event_write_stream* stream);

#endif
