#include "buffered_socket.h"

#include <tyranscript/tyran_types.h>

#include "../stream/out_stream.h"
#include "ring_buffer.h"
#include "connecting_socket.h"
#include "../event/event_listener.h"

typedef struct nimbus_buffered_socket {
	nimbus_ring_buffer* buffer;
	nimbus_out_stream* out_stream;
	u8t* in_buffer;
	int in_buffer_size;
	nimbus_event_listener;
} nimbus_buffered_socket;


typedef void (*nimbus_event_read)(void* self, struct nimbus_event_read_stream* stream);


static on_resource_request(nimbus_buffered_socket* self, nimbus_resource_id resource_id)
{
	TYRAN_LOG("ON Request %d", resource_id);
	nimbus_buffered_socket_send_request(self, resource_id);
}

static _on_resource_request(void* _self, struct nimbus_event_read_stream* stream)
{
	nimbus_buffered_socket* self = (nimbus_buffered_socket*) _self;
	nimbus_resource_id resource_id = nimbus_resource_id_from_stream(stream);
	
	on_resource_request(self, resource_id);
}



nimbus_buffered_socket* nimbus_buffered_socket_new(tyran_memory* memory, connecting_socket* socket)
{
	nimbus_buffered_socket* self = TYRAN_MEMORY_CALLOC_TYPE(memory, nimbus_buffered_socket);
	self->socket = socket;
	self->buffer = nimbus_ring_buffer_new(memory, 1024);
	self->out_stream = nimbus_out_stream_new(memory, 1024);
	self->out_event_stream = nimbus_event_stream_new(memory, 1024);
	self->in_stream = nimbus_in_stream_new(memory, 1024);
	
	self->listener = nimbus_event_listener_new(memory);

	nimbus_event_listener_listen(self->listener, RESOURCE_REQUEST_ID, _on_resource_request);


	return self;
}

nimbus_buffered_socket_free(nimbus_buffered_socket* self)
{
	nimbus_ring_buffer_free(self->buffer);
	nimbus_out_stream_free(self->out_stream);
	nimbus_out_event_stream_free(self->out_event_stream);
	nimbus_in_stream_free(self->in_stream);
	TYRAN_MEMORY_FREE(self);
}

void run(nimbus_buffered_socket* self)
{
	while (1) {
		int octets_read = receive(self);
		if (octets_read == -1) {
			break;
		}
	}
}

static void handle_deleted(nimbus_buffered_socket* self, nimbus_resource_id resource_id)
{
	self->expected_payload_size = 0;
}

static void handle_updated(nimbus_buffered_socket* self, nimbus_in_stream* in_stream, nimbus_resource_id resource_id)
{
	nimbus_in_stream_read_u32(in_stream, &self->expected_payload_size);
}

static void read_message_type(nimbus_buffered_socket* self, nimbus_in_stream* in_stream)
{
	u8t message_type;
	nimbus_in_stream_read_u8(in_stream, &message_type);
	nimbus_resource_id resource_id = nimbus_resource_id_from_stream(in_stream);
	switch (message_type) {
		case 0: // Delete
			handle_deleted(self);
		break;
		case 1: // New
		case 2: // Update
			handle_updated(self, in_stream, resource_id);
		break;
	}
}

void check_header(nimbus_buffered_socket* self)
{
	int buffer_size = nimbus_ring_buffer_size(self->buffer);
	const int header_size = 8;
	if (temp_buffer_size >= header_size) {
		u8t temp_buffer[header_size];
		
		nimbus_ring_buffer_read(self->buffer, temp_buffer, header_size);	
		nimbus_in_stream_init(self->in_stream, temp_buffer, header_size);
		read_message_type(self, self->in_stream);
		self->waiting_for_header = 0;	
	}
	
}

static void on_payload_done(nimbus_buffered_socket* self)
{
	u8t* temp_buffer;
	int temp_buffer_size;
	
	nimbus_resource_id resource_id;
	
	nimbus_event_stream_write_event_header(self->out_event_stream, RESOURCE_UPDATED, self->expected_payload_size + sizeof(resource_id));
	nimbus_event_stream_write_type(self->out_event_stream, self->resource_id);

	nimbus_ring_buffer_read_pointer(self->buffer, self->expected_payload_size, &temp_buffer, &temp_buffer_size);
	self->expected_payload_size -= temp_buffer_size;
	nimbus_event_stream_write_octets(self->out_event_stream, temp_buffer, temp_buffer_size);

	nimbus_ring_buffer_read_pointer(self->buffer, self->expected_payload_size, &temp_buffer, &temp_buffer_size);
	self->expected_payload_size -= temp_buffer_size;
	nimbus_event_stream_write_octets(self->out_event_stream, temp_buffer, temp_buffer_size);
	
	self->waiting_for_header = 1;
}

void receive(nimbus_buffered_socket* self)
{
	u8t* temp_buffer;
	int temp_buffer_size;
	
	nimbus_ring_buffer_write_pointer(self->buffer, &temp_buffer, &temp_buffer_size);
	
	int octets_read = nimbus_connecting_socket_read(self->socket, temp_buffer, temp_buffer_size);
	if (octets_read > 0) {
		nimbus_ring_buffer_write_pointer_advance(self->buffer, octets_read);
		if (self->waiting_for_header) {
			check_header(self, buffer_size);	
		} else {
			int buffer_size = nimbus_ring_buffer_size(self->buffer);
			if (buffer_size >= self->expected_payload_size) {
				on_payload_done(self);
			}
		}
	}
}

void nimbus_buffered_socket_run(void* _self)
{
	nimbus_buffered_socket* self = (nimbus_buffered_socket*) _self;
	
	run(self);
}

void nimbus_buffered_socket_send_request(nimbus_buffered_socket* self, nimbus_resource_id resource_id)
{
	nimbus_out_stream_clear(self->out_stream);
	nimbus_out_stream_write_u8(self->out_stream, request_id);
	nimbus_out_stream_write_u32(self->out_stream, resource_id);
	
	const u8t* buffer;
	int length;
	nimbus_out_stream_info(self->out_stream, &buffer, &length);
	nimbus_connecting_socket_write(self->socket, buffer, length);
}
