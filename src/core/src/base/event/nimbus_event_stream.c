#include <tyran_core/event/event_stream.h>
#include <tyranscript/tyran_clib.h>
#include <tyranscript/tyran_string.h>
#include <tyranscript/tyran_symbol_table.h>

void nimbus_event_write_stream_clear(nimbus_event_write_stream* self)
{
	self->pointer = self->buffer;
}

void nimbus_event_write_stream_init(nimbus_event_write_stream* self, tyran_memory* memory, int max_length)
{
	self->buffer = TYRAN_MEMORY_ALLOC(memory, max_length, "event write buffer");
	nimbus_event_write_stream_clear(self);
	self->end_pointer = self->buffer + max_length;

}

void nimbus_event_write_stream_free(nimbus_event_write_stream* stream)
{
	TYRAN_MEMORY_FREE(stream->buffer);
}

int nimbus_event_write_stream_length(nimbus_event_write_stream* self)
{
	return self->pointer - self->buffer;
}

void nimbus_event_stream_write_octets(nimbus_event_write_stream* stream, const void* data, int len)
{
	TYRAN_ASSERT(stream->pointer + len < stream->end_pointer, "Event Stream Overwrite. Len:%d", len);
	tyran_memcpy_type(u8t, stream->pointer, data, len);
	stream->pointer += len;
}

void write_skip(nimbus_event_write_stream* self, int length)
{
	self->pointer += length;
}

void nimbus_event_stream_write_align(nimbus_event_write_stream* self)
{
	const int alignment = 4;
	if (((tyran_pointer_to_number)self->pointer % alignment) != 0) {
		write_skip(self, alignment - ((tyran_pointer_to_number)self->pointer % alignment));
	}
}

void nimbus_event_stream_write_string(nimbus_event_write_stream* self, const tyran_string* string)
{
	nimbus_event_stream_write_octets(self, (const u8t*)&string->len, sizeof(string->len));
	nimbus_event_stream_write_octets(self, string->buf, sizeof(tyran_string_char) * string->len);
}

void nimbus_event_stream_write_event_end(nimbus_event_write_stream* self)
{
	int octet_size = self->pointer - ((u8t*)self->last_header) - sizeof(nimbus_event_stream_header);
	self->last_header->event_octet_size = octet_size;
}

void nimbus_event_stream_read_pointer(nimbus_event_read_stream* stream, const u8t** data, int len)
{
	TYRAN_ASSERT(stream->pointer + len <= stream->end_pointer, "Read too far");
	*data = stream->pointer;
	stream->pointer += len;
}

void nimbus_event_stream_read_octets(nimbus_event_read_stream* stream, u8t* data, int len)
{
	TYRAN_ASSERT(stream->pointer + len <= stream->end_pointer, "Read too far");
	tyran_memcpy_type(u8t, data, stream->pointer, len);
	stream->pointer += len;
}

void nimbus_event_stream_read_init(nimbus_event_read_stream* self, const struct tyran_symbol_table* table, const u8t* pointer, int length)
{
	self->pointer = pointer;
	self->end_pointer = pointer + length;
	self->symbol_table = table;
}

void nimbus_event_stream_read_skip(nimbus_event_read_stream* self, int length)
{
	TYRAN_ASSERT(self->pointer + length <= self->end_pointer, "Read skipped too far");
	self->pointer += length;
}

void nimbus_event_stream_read_string(nimbus_event_read_stream* self, struct tyran_memory* memory, tyran_string* string)
{
	tyran_string_length_type character_count;
	nimbus_event_stream_read_octets(self, (u8t*)(&character_count), sizeof(character_count));
	const u8t* data;
	nimbus_event_stream_read_pointer(self, &data, character_count * sizeof(tyran_string_char));
	tyran_string_init(string, memory, (tyran_string_char*)data, character_count);
}

const char* nimbus_event_stream_read_convert_symbol_string(nimbus_event_read_stream* self, const struct tyran_symbol* symbol)
{
	return tyran_symbol_table_lookup(self->symbol_table, symbol);
}

int nimbus_event_stream_read_array(nimbus_event_read_stream* self, void* destination, int size_of_entity, int max_array_count)
{
	const u8t* d = self->pointer;
	
	int count_in_array = *((int*)d);
	d += sizeof(int);
	
	TYRAN_ASSERT(count_in_array <= max_array_count, "Can not read big array:%d", count_in_array);

	tyran_memcpy_octets(destination, d, size_of_entity * count_in_array);
	nimbus_event_stream_read_skip(self, size_of_entity * count_in_array);
	
	return count_in_array;
}

void nimbus_event_stream_read_align(nimbus_event_read_stream* self)
{
	const int alignment = 4;
	if (((tyran_pointer_to_number)self->pointer % alignment) != 0) {
		nimbus_event_stream_read_skip(self, alignment - ((tyran_pointer_to_number)self->pointer % alignment) );
	}
}
