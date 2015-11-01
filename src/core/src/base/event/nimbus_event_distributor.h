#ifndef nimbus_event_distributor_h
#define nimbus_event_distributor_h

struct tyran_memory;
struct nimbus_update;
struct tyran_symbol_table;

typedef struct nimbus_event_distributor {
	unsigned char* event_buffer;
	int event_buffer_size;
	int event_buffer_used_octet_size;
	const struct tyran_symbol_table* symbol_table;
} nimbus_event_distributor;

void nimbus_event_distributor_init(nimbus_event_distributor* self, const struct tyran_symbol_table* table,
								   struct tyran_memory* memory);
void nimbus_event_distributor_distribute_events(nimbus_event_distributor* self, struct nimbus_update** objects, int object_count);

#endif
