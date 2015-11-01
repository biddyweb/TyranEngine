#include <tyran_core/event/event_listener.h>
#include <tyran_core/event/event_stream.h>
#include <tyranscript/tyran_memory.h>
#include <tyranscript/tyran_log.h>

nimbus_event_listener_function* nimbus_event_function_from_event_id(nimbus_event_listener* self, nimbus_event_type_id id)
{
	for (int i = 0; i < self->function_count; ++i) {
		nimbus_event_listener_function* func = &self->functions[i];
		if (func->id == id) {
			return func;
		}
	}

	return 0;
}

void nimbus_event_process(nimbus_event_listener* self, struct nimbus_event_read_stream* read_stream)
{
	void* other_self = self->other_self;

	nimbus_event_stream_header header;

	for (; read_stream->pointer < read_stream->end_pointer;) {
		nimbus_event_stream_read_type(read_stream, header);
		// TYRAN_LOG("Read event header type:%d octet_size:%d", header.event_type_id, header.event_octet_size);

		nimbus_event_listener_function* func = nimbus_event_function_from_event_id(self, header.event_type_id);

		if (func) {
			nimbus_event_read_stream stream;
			nimbus_event_stream_read_init(&stream, read_stream->symbol_table, read_stream->pointer, header.event_octet_size);
			func->event_reader(other_self, &stream);
		} else {
			// TYRAN_LOG("No specific listener");
		}

		if (self->listen_to_all) {
			nimbus_event_read_stream stream;
			nimbus_event_stream_read_init(&stream, read_stream->symbol_table, read_stream->pointer, header.event_octet_size);
			stream.event_type_id = header.event_type_id;
			self->listen_to_all(other_self, &stream);
		}

		nimbus_event_stream_read_skip(read_stream, header.event_octet_size);
	}
}

void nimbus_event_listener_init(nimbus_event_listener* self, void* other_self)
{
	self->other_self = other_self;
	self->function_count = 0;
	self->listen_to_all = 0;
}

void nimbus_event_listener_listen(nimbus_event_listener* self, nimbus_event_type_id id, nimbus_event_read reader)
{
	self->functions[self->function_count].event_reader = reader;
	self->functions[self->function_count].id = id;
	self->function_count++;
}

void nimbus_event_listener_listen_to_all(nimbus_event_listener* self, nimbus_event_read reader)
{
	self->listen_to_all = reader;
}
