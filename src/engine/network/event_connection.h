#ifndef nimbus_event_connection_h
#define nimbus_event_connection_h

#include "connecting_socket.h"
#include "ring_buffer.h"
#include "../../core/src/base/event/nimbus_event_listener.h"

#include "../stream/out_stream.h"
#include "../stream/in_stream.h"

#include "../resource/resource_id.h"


struct tyran_memory;

typedef struct nimbus_event_connection {
	nimbus_ring_buffer buffer;
	nimbus_out_stream out_stream;
	nimbus_in_stream in_stream;
	u8t* in_buffer;
	int in_buffer_size;
	nimbus_event_listener event_listener;
	nimbus_event_write_stream out_event_stream;
	nimbus_connecting_socket socket;
	nimbus_resource_id resource_id;

	u32t expected_payload_size;
	int waiting_for_header;
} nimbus_event_connection;


struct nimbus_event_connection* nimbus_event_connection_new(struct tyran_memory* memory, struct nimbus_connecting_socket* socket);
void nimbus_event_connection_run(void* _self);

#endif
