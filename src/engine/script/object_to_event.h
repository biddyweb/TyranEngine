#ifndef nimbus_object_to_event_h
#define nimbus_object_to_event_h

#include "property_reader.h"

typedef struct nimbus_object_to_event {
	nimbus_property_reader property_reader;
	u8t* temp_buf;
	int temp_buf_max_size;
} nimbus_object_to_event;

#endif
