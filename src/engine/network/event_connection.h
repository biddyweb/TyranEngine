#ifndef nimbus_event_connection_h
#define nimbus_event_connection_h

#include "connecting_socket.h"
#include "ring_buffer.h"
#include <tyran_core/event/event_listener.h>

#include "../stream/out_stream.h"
#include "../stream/in_stream.h"

#include <tyran_engine/resource/id.h>
#include <tyran_engine/resource/type_id.h>
#include <tyran_core/update/update.h>
#include "../../core/src/base/mutex/nimbus_mutex.h"

struct tyran_memory;

typedef struct nimbus_event_connection {
	nimbus_ring_buffer buffer;
	nimbus_out_stream out_stream;
	nimbus_in_stream in_stream;
	u8t* in_buffer;
	int in_buffer_size;

	//	nimbus_event_write_stream out_event_stream;

	nimbus_update update_object;

	nimbus_task receive_task;

	nimbus_connecting_socket socket;
	nimbus_resource_id resource_id;
	nimbus_resource_type_id resource_type_id;

	nimbus_mutex ring_buffer_mutex;

	u32t expected_payload_size;
	int waiting_for_header;
} nimbus_event_connection;

void nimbus_event_connection_init(nimbus_event_connection* self, struct tyran_memory* memory, const char* host, int port);

#endif
