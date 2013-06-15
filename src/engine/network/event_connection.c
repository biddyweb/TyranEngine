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

void nimbus_event_connection_send_request(nimbus_event_connection* self, nimbus_resource_id resource_id)
{
	nimbus_out_stream_clear(&self->out_stream);
	nimbus_out_stream_write_u8(&self->out_stream, 2);
	nimbus_out_stream_write_u32(&self->out_stream, resource_id);

	const u8t* buffer;
	int length;
	nimbus_out_stream_info(&self->out_stream, &buffer, &length);
	nimbus_connecting_socket_write(&self->socket, buffer, length);
}

static void on_resource_request(nimbus_event_connection* self, nimbus_resource_id resource_id)
{
	TYRAN_LOG("ON Request %d", resource_id);
	nimbus_event_connection_send_request(self, resource_id);
}

static void _on_resource_request(void* _self, struct nimbus_event_read_stream* stream)
{
	nimbus_event_connection* self = (nimbus_event_connection*) _self;
	nimbus_resource_id resource_id = nimbus_resource_id_from_stream(stream);

	on_resource_request(self, resource_id);
}

void nimbus_event_connection_init(nimbus_event_connection* self, tyran_memory* memory, const char* host, int port)
{
	TYRAN_LOG("Booting event connection");
	nimbus_ring_buffer_init(&self->buffer, memory, 1024);
	nimbus_out_stream_init(&self->out_stream, memory, 1024);
	nimbus_connecting_socket_init(&self->socket, host, port);
	nimbus_update_init(&self->update_object, memory);

	nimbus_event_listener_listen(&self->update_object.event_listener, NIMBUS_EVENT_RESOURCE_LOAD, _on_resource_request);
}

void nimbus_event_connection_free(nimbus_event_connection* self)
{
	nimbus_ring_buffer_free(&self->buffer);
	nimbus_out_stream_free(&self->out_stream);
}

static void handle_deleted(nimbus_event_connection* self)
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
	u8t* temp_buffer;
	int temp_buffer_size;

	nimbus_resource_id resource_id;

	const int RESOURCE_UPDATED = 2;

	nimbus_event_write_stream_clear(&self->out_event_stream);
	nimbus_event_stream_write_event_header(&self->out_event_stream, RESOURCE_UPDATED, self->expected_payload_size + sizeof(resource_id));
	nimbus_event_stream_write_type(&self->out_event_stream, self->resource_id);

	nimbus_ring_buffer_read_pointer(&self->buffer, self->expected_payload_size, &temp_buffer, &temp_buffer_size);
	self->expected_payload_size -= temp_buffer_size;
	nimbus_event_stream_write_octets(&self->out_event_stream, temp_buffer, temp_buffer_size);

	nimbus_ring_buffer_read_pointer(&self->buffer, self->expected_payload_size, &temp_buffer, &temp_buffer_size);
	self->expected_payload_size -= temp_buffer_size;
	nimbus_event_stream_write_octets(&self->out_event_stream, temp_buffer, temp_buffer_size);

	self->waiting_for_header = 1;
}


static int receive(nimbus_event_connection* self)
{
	u8t* temp_buffer;
	int temp_buffer_size;

	nimbus_ring_buffer_write_pointer(&self->buffer, &temp_buffer, &temp_buffer_size);

	int octets_read = nimbus_connecting_socket_read(&self->socket, temp_buffer, temp_buffer_size);
	if (octets_read > 0) {
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


void run(nimbus_event_connection* self)
{
	while (1) {
		int octets_read = receive(self);
		if (octets_read == -1) {
			break;
		}
	}
}





void nimbus_event_connection_run(void* _self)
{
	nimbus_event_connection* self = (nimbus_event_connection*) _self;

	run(self);
}

