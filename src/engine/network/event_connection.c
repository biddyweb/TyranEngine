#if defined TORNADO_OS_IOS

#include "event_connection.h"

#include <tyranscript/tyran_types.h>

#include "../stream/out_stream.h"
#include "../stream/in_stream.h"
#include "ring_buffer.h"
#include "connecting_socket.h"
#include <tyran_engine/resource/id.h>
#include "../resource/resource_reader.h"
#include <tyran_core/event/event_stream.h>
#include <tyran_engine/event/resource_load.h>
#include <tyran_engine/event/resource_updated.h>
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
	nimbus_out_stream_clear(&self->out_stream);
	const u8t request_resource_id = 8;
	nimbus_out_stream_write_u8(&self->out_stream, request_resource_id);
	nimbus_out_stream_write_u32(&self->out_stream, resource_id);
	send_stream(self);
}

static void on_resource_request(nimbus_event_connection* self, nimbus_resource_id resource_id)
{
	send_request(self, resource_id);
}

static void _on_resource_request(void* _self, struct nimbus_event_read_stream* stream)
{
	nimbus_event_connection* self = (nimbus_event_connection*) _self;
	nimbus_resource_id resource_id = nimbus_resource_id_from_stream(stream);

	on_resource_request(self, resource_id);
}

void nimbus_event_connection_free(nimbus_event_connection* self)
{
	tyran_free(self->in_buffer);
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

static int read_resource_type_id(nimbus_in_stream* stream, nimbus_resource_type_id* resource_type_id)
{
	return nimbus_in_stream_read_u32(stream, resource_type_id);
}

static int read_resource_id(nimbus_in_stream* stream, nimbus_resource_id* resource_id)
{
	return nimbus_in_stream_read_u32(stream, resource_id);
}

static void read_message_type(nimbus_event_connection* self, nimbus_in_stream* in_stream)
{
	u8t message_type;
	nimbus_in_stream_read_u8(in_stream, &message_type);
	read_resource_id(in_stream, &self->resource_id);
	read_resource_type_id(in_stream, &self->resource_type_id);

	switch (message_type) {
		case 0: // Delete
			handle_deleted(self, self->resource_id);
			break;
		case 1: // New
		case 9: // Update
			handle_updated(self, in_stream, self->resource_id);
			break;
	}
}

void check_header(nimbus_event_connection* self)
{
	int buffer_size = nimbus_ring_buffer_size(&self->buffer);
	const int header_size = 13;
	if (buffer_size >= header_size) {
		u8t temp_buffer[header_size];

		nimbus_ring_buffer_read(&self->buffer, temp_buffer, header_size);
		nimbus_in_stream_init(&self->in_stream, temp_buffer, header_size);
		read_message_type(self, &self->in_stream);
		self->waiting_for_header = 0;
	}
}

static void fire_resource_updated(nimbus_event_write_stream* out_event_stream, nimbus_resource_id resource_id, nimbus_resource_type_id resource_type_id, nimbus_ring_buffer* buffer, int expected_payload_size)
{
	u8t* temp_buffer;
	int temp_buffer_size;

	TYRAN_LOG("EventConnection: Fire resource id:%d resource_name:'%s' type:%d", resource_id, nimbus_resource_id_debug_name(resource_id), resource_type_id);

	nimbus_resource_updated resource_updated;
	resource_updated.resource_id = resource_id;
	resource_updated.resource_type_id = resource_type_id;
	resource_updated.payload_size = expected_payload_size;

	nimbus_event_stream_write_event_header(out_event_stream, NIMBUS_EVENT_RESOURCE_UPDATED);
	nimbus_event_stream_write_type(out_event_stream, resource_updated);

	nimbus_ring_buffer_read_pointer(buffer, &temp_buffer, &temp_buffer_size);
	int read_count = expected_payload_size > temp_buffer_size ? temp_buffer_size : expected_payload_size;
	nimbus_ring_buffer_read_pointer_advance(buffer, read_count);
	expected_payload_size -= read_count;
	nimbus_event_stream_write_octets(out_event_stream, temp_buffer, read_count);

	nimbus_ring_buffer_read_pointer(buffer, &temp_buffer, &temp_buffer_size);
	read_count = expected_payload_size > temp_buffer_size ? temp_buffer_size : expected_payload_size;
	nimbus_ring_buffer_read_pointer_advance(buffer, read_count);
	expected_payload_size -= read_count;
	nimbus_event_stream_write_octets(out_event_stream, temp_buffer, read_count);

	TYRAN_ASSERT(expected_payload_size == 0, "Couldn't write everything from buffer");
	nimbus_event_stream_write_event_end(out_event_stream);
}

static void on_payload_done(nimbus_event_connection* self)
{
	self->waiting_for_header = 1;
	fire_resource_updated(&self->update_object.event_write_stream, self->resource_id, self->resource_type_id, &self->buffer, self->expected_payload_size);
}

static void consume(nimbus_event_connection* self)
{
	int buffer_size = nimbus_ring_buffer_size(&self->buffer);
	if (self->waiting_for_header) {
		if (buffer_size >= 13) {
			check_header(self);
			consume(self);
		} else {
		}
	} else {
		if (buffer_size >= (int) self->expected_payload_size) {
			on_payload_done(self);
			consume(self);
		} else {
		}
	}
}

static void on_update(nimbus_event_connection* self)
{
	nimbus_mutex_lock(&self->ring_buffer_mutex);
	consume(self);
	nimbus_mutex_unlock(&self->ring_buffer_mutex);
}

static void _on_update(void* self)
{
	on_update(self);
}

static int receive(nimbus_event_connection* self)
{
	int octets_read = nimbus_connecting_socket_read(&self->socket, self->in_buffer, self->in_buffer_size);
	if (octets_read > 0) {
		nimbus_mutex_lock(&self->ring_buffer_mutex);
		nimbus_ring_buffer_write(&self->buffer, self->in_buffer, octets_read);
		nimbus_mutex_unlock(&self->ring_buffer_mutex);
	}

	return octets_read;
}

static void receive_task(void* _self, struct nimbus_task_queue* task_queue)
{
	nimbus_event_connection* self = _self;
	while (1) {
		int octets_read = receive(self);
		if (octets_read == -1) {
			TYRAN_LOG("Receive -1");
			break;
		}
	}
}

void nimbus_event_connection_init(nimbus_event_connection* self, tyran_memory* memory, const char* host, int port)
{
	self->waiting_for_header = 1;
	// Ring buffer must be big enough to hold the largest resource file.

	self->in_buffer_size = 8 * 1024;
	self->in_buffer = TYRAN_MEMORY_ALLOC(memory, self->in_buffer_size, "event connection buffer");

	const int largest_resource_file = 6 * 1024 * 1024;
	nimbus_ring_buffer_init(&self->buffer, memory, largest_resource_file);

	nimbus_out_stream_init(&self->out_stream, memory, largest_resource_file + 1024);

	nimbus_connecting_socket_init(&self->socket, host, port);
	send_connect(self);
	nimbus_update_init_ex(&self->update_object, memory, _on_update, self, largest_resource_file, "connection");
	nimbus_task_init(&self->receive_task, receive_task, self, "event_connection_receive_task");

	nimbus_event_listener_init(&self->update_object.event_listener, self);
	nimbus_event_listener_listen(&self->update_object.event_listener, NIMBUS_EVENT_RESOURCE_LOAD, _on_resource_request);
	nimbus_mutex_init(&self->ring_buffer_mutex);
}
#else

int x;
#endif
