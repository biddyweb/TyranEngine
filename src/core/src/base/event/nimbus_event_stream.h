#ifndef nimbus_event_stream_h
#define nimbus_event_stream_h

#include <tyranscript/tyran_types.h>

struct tyran_memory;

typedef u8t nimbus_event_type_id;

typedef struct nimbus_event_write_stream {
	u8t* buffer;
	u8t* pointer;
	u8t* end_pointer;
} nimbus_event_write_stream;

typedef struct nimbus_event_read_stream {
	const u8t* pointer;
	nimbus_event_type_id event_type_id;
} nimbus_event_read_stream;

void nimbus_event_write_stream_init(nimbus_event_write_stream* stream, struct tyran_memory* memory, int max_length);
void nimbus_event_write_stream_free(nimbus_event_write_stream* stream);
void nimbus_event_write_stream_clear(nimbus_event_write_stream* self);

void nimbus_event_stream_write_octets(nimbus_event_write_stream* stream, const void* data, int len);
void nimbus_event_stream_read_octets(nimbus_event_read_stream* stream, u8t* data, int len);

#define nimbus_event_stream_write_type(stream, variable) {  nimbus_event_stream_write_octets(stream, (const u8t*)(&variable), sizeof(variable)); }
#define nimbus_event_stream_write_event_header(stream, event_type_id, len) { nimbus_event_stream_write_type(stream, event_type_id); nimbus_event_stream_write_type(stream, len); }
#define nimbus_event_stream_write_event(stream, event_type_id, variable) { u16t len = sizeof(variable); nimbus_event_stream_write_event_header(stream, event_type_id, len); nimbus_event_stream_write_type(stream, variable); }
#define nimbus_event_stream_read_type(stream, variable) nimbus_event_stream_read_octets(stream, (u8t*)(&variable), sizeof(variable));


#endif
