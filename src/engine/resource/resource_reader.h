#ifndef nimbus_resource_reader_h
#define nimbus_resource_reader_h

#include <tyran_engine/resource/id.h>

struct nimbus_event_listener;
struct tyran_memory;
struct nimbus_update;

typedef struct nimbus_resource_reader {
	struct nimbus_event_listener event_listener;
	struct nimbus_resource_handler* resource_handler;
	struct nimbus_update* update_object;

	int temporary_file_buffer_size;
	int temporary_file_buffer_max_size;
	u8t* temporary_file_buffer;

} nimbus_resource_reader;

nimbus_resource_reader* nimbus_resource_reader_new(struct tyran_memory* memory, struct nimbus_resource_handler* resource_handler);
nimbus_resource_id nimbus_resource_id_from_stream(nimbus_event_read_stream* stream);

#endif
