#include "event_connection.h"

#include <tyranscript/tyran_types.h>

#include "../stream/out_stream.h"
#include "../stream/in_stream.h"
#include "ring_buffer.h"
#include "connecting_socket.h"
#include "../resource/resource_id.h"
#include "../resource/resource_reader.h"
#include "../../core/src/base/event/nimbus_event_stream.h"
#include "../event/resource_load.h"
#include <tyranscript/tyran_log.h>

void send_stream(nimbus_event_connection* self)
{
	const u8t* buffer;
	int length;
	nimbus_out_stream_info(&self->out_stream, &buffer, &length);
	nimbus_connecting_socket_write(&self->socket, buffer, length);
}


static void send_connect(nimbus_event_connection* self)
{
	nimbus_out_stream_clear(&self->out_stream);

	const u8t connect_id = 99;
	nimbus_out_stream_write_u8(&self->out_stream, connect_id);

	const u8t protocol_type_game = 0x01;
	nimbus_out_stream_write_u8(&self->out_stream, protocol_type_game);

	const u32t version = 0x01;
	nimbus_out_stream_write_u32(&self->out_stream, version);

	send_stream(self);
}


void send_request(nimbus_event_connection* self, nimbus_resource_id resource_id)
{
	TYRAN_LOG("send_request resource_id:%d", resource_id);
	nimbus_out_stream_clear(&self->out_stream);
	const u8t request_resource_id = 8;
	nimbus_out_stream_write_u8(&self->out_stream, request_resource_id);
	nimbus_out_stream_write_u32(&self->out_stream, resource_id);
	send_stream(self);
}

static void on_resource_request(nimbus_event_connection* self, nimbus_resource_id resource_id)
{
	TYRAN_LOG("ON Request %d", resource_id);
	send_request(self, resource_id);
}

static void _on_resource_request(void* _self, struct nimbus_event_read_stream* stream)
{
	nimbus_event_connection* self = (nimbus_event_connection*) _self;
	nimbus_resource_id resource_id = nimbus_resource_id_from_stream(stream);
	
	TYRAN_LOG("on_resource_request:%d", resource_id);

	on_resource_request(self, resource_id);
}


void _on_update(void* self)
{
	TYRAN_LOG("connection.update");
}


void nimbus_event_connection_free(nimbus_event_connection* self)
{
	nimbus_ring_buffer_free(&self->buffer);
	nimbus_out_stream_free(&self->out_stream);
}

static void handle_deleted(nimbus_event_connection* self, nimbus_resource_id resource_id)
{
	self->expected_payload_size = 0;
}

static void handle_updated(nimbus_event_connection* self, nimbus_in_stream* in_stream, nimbus_resource_id resource_id)
{
	nimbus_in_stream_read_u32(in_stream, &self->expected_payload_size);
}

static int read_resource_id(nimbus_in_stream* stream, nimbus_resource_id* resource_id)
{
	return nimbus_in_stream_read_u32(stream, resource_id);
}

static void read_message_type(nimbus_event_connection* self, nimbus_in_stream* in_stream)
{
	u8t message_type;
	nimbus_in_stream_read_u8(in_stream, &message_type);
	nimbus_resource_id resource_id;
	read_resource_id(in_stream, &resource_id);
	TYRAN_LOG("read_message_type:%d resource_id:%d", message_type, resource_id);
	switch (message_type) {
		case 0: // Delete
			handle_deleted(self, resource_id);
			break;
		case 1: // New
		case 9: // Update
			handle_updated(self, in_stream, resource_id);
			break;
	}
}


void check_header(nimbus_event_connection* self)
{
	int buffer_size = nimbus_ring_buffer_size(&self->buffer);
	const int header_size = 8;
	if (buffer_size >= header_size) {
		u8t temp_buffer[header_size];

		nimbus_ring_buffer_read(&self->buffer, temp_buffer, header_size);
		nimbus_in_stream_init(&self->in_stream, temp_buffer, header_size);
		read_message_type(self, &self->in_stream);
		self->waiting_for_header = 0;
	}

}


static void on_payload_done(nimbus_event_connection* self)
{
	TYRAN_LOG("Payload received!");
	u8t* temp_buffer;
	int temp_buffer_size;

	nimbus_resource_id resource_id;

	const int RESOURCE_UPDATED = 2;

	nimbus_event_write_stream_clear(&self->out_event_stream);
	nimbus_event_stream_write_event_header(&self->out_event_stream, RESOURCE_UPDATED, self->expected_payload_size + sizeof(resource_id));
	nimbus_event_stream_write_type(&self->out_event_stream, self->resource_id);

	nimbus_ring_buffer_read_pointer(&self->buffer, self->expected_payload_size, &temp_buffer, &temp_buffer_size);
	TYRAN_LOG("Ring bufferA:%d", temp_buffer_size);
	self->expected_payload_size -= temp_buffer_size;
	nimbus_event_stream_write_octets(&self->out_event_stream, temp_buffer, temp_buffer_size);

	nimbus_ring_buffer_read_pointer(&self->buffer, self->expected_payload_size, &temp_buffer, &temp_buffer_size);
	TYRAN_LOG("Ring bufferB:%d", temp_buffer_size);
	self->expected_payload_size -= temp_buffer_size;
	nimbus_event_stream_write_octets(&self->out_event_stream, temp_buffer, temp_buffer_size);

	self->waiting_for_header = 1;
}


static int receive(nimbus_event_connection* self)
{
	u8t* temp_buffer;
	int temp_buffer_size;

	nimbus_ring_buffer_write_pointer(&self->buffer, &temp_buffer, &temp_buffer_size);
	TYRAN_LOG("write_pointer %d", temp_buffer_size);
	int octets_read = nimbus_connecting_socket_read(&self->socket, temp_buffer, temp_buffer_size);
	if (octets_read > 0) {
		TYRAN_LOG("Received octets:%d (%c)", octets_read, temp_buffer[0]);
		nimbus_ring_buffer_write_pointer_advance(&self->buffer, octets_read);
		if (self->waiting_for_header) {
			check_header(self);
		} else {
			int buffer_size = nimbus_ring_buffer_size(&self->buffer);
			if (buffer_size >= (int) self->expected_payload_size) {
				on_payload_done(self);
			}
		}
	}

	return octets_read;
}

static void receive_task(void* _self, struct nimbus_task_queue* task_queue)
{
	TYRAN_LOG("receive task!");
	nimbus_event_connection* self = _self;
	while (1) {
		TYRAN_LOG("Receive loop");
		int octets_read = receive(self);
		if (octets_read == -1) {
			TYRAN_LOG("Receive -1");
			break;
		}
	}
}

void nimbus_event_connection_init(nimbus_event_connection* self, tyran_memory* memory, const char* host, int port)
{
	TYRAN_LOG("Booting event connection");
	nimbus_ring_buffer_init(&self->buffer, memory, 1024);
	nimbus_out_stream_init(&self->out_stream, memory, 1024);
	
	nimbus_event_write_stream_init(&self->out_event_stream, memory, 1024);
	nimbus_connecting_socket_init(&self->socket, host, port);
	send_connect(self);
	nimbus_update_init(&self->update_object, memory, _on_update, self);
	
	nimbus_task_init(&self->receive_task, receive_task, self);

	nimbus_event_listener_init(&self->update_object.event_listener, self);
	nimbus_event_listener_listen(&self->update_object.event_listener, NIMBUS_EVENT_RESOURCE_LOAD, _on_resource_request);
}

