#include <tyranscript/tyran_memory.h>
#include "../../core/src/base/event/nimbus_event_listener.h"
#include "resource_reader.h"
#include "resource_handler.h"
#include "../../core/src/base/update/nimbus_update.h"
#include "../../core/src/base/event/nimbus_event_stream.h"

#include <stdio.h>

#define nimbus_event_resource_request_id 1

static void nimbus_resource_reader_read_file(nimbus_resource_reader* self, nimbus_resource_id id)
{
	const char* name = nimbus_resource_handler_id_to_name(self->resource_handler, id);
	FILE* file = fopen(name, "rb");
	self->temporary_file_buffer_size = fread(self->temporary_file_buffer, self->temporary_file_buffer_max_size, 1, file);
	fclose(file);
}

void nimbus_resource_reader_fire_resource(nimbus_resource_reader* self, nimbus_resource_id id)
{
	nimbus_event_write_stream* stream = &self->update_object->event_write_stream;

	//nimbus_event_stream_write_header(&stream, nimbus_event_resource_loaded_id, self->temporary_file_buffer_size + sizeof(nimbus_resource_id));
	nimbus_event_stream_write_event_header(stream, id, self->temporary_file_buffer_size);
	nimbus_event_stream_write_octets(stream, self->temporary_file_buffer, self->temporary_file_buffer_size);
}

void nimbus_resource_reader_on_request(void* _self, nimbus_event_read_stream* stream)
{
	nimbus_resource_reader* self = _self;
	nimbus_resource_id id = nimbus_resource_id_from_stream(stream);
	nimbus_resource_reader_read_file(self, id);
	nimbus_resource_reader_fire_resource(self, id);
	self->temporary_file_buffer_size = 0;
}

nimbus_resource_id nimbus_resource_id_from_stream(nimbus_event_read_stream* stream)
{
	nimbus_resource_id id;

	nimbus_event_stream_read_type(stream, id);

	return id;
}


nimbus_resource_reader* nimbus_resource_reader_new(tyran_memory* memory, nimbus_resource_handler* resource_handler)
{
	nimbus_resource_reader* self = TYRAN_MEMORY_CALLOC_TYPE(memory, nimbus_resource_reader);
	nimbus_event_listener_init(self->event_listener);
	self->resource_handler = resource_handler;
	nimbus_event_listener_listen(self->event_listener, nimbus_event_resource_request_id, nimbus_resource_reader_on_request);

	return self;
}



