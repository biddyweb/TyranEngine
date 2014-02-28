#ifndef nimbus_state_update_send_h
#define nimbus_state_update_send_h

struct nimbus_component_arrays;
struct nimbus_event_write_stream;

void nimbus_state_update_send(struct nimbus_component_arrays* arrays, struct nimbus_event_write_stream* stream);

#endif
