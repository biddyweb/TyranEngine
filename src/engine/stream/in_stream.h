#ifndef nimbus_engine_in_stream_h
#define nimbus_engine_in_stream_h

typedef struct in_stream {
    const u8t* buffer;
    const u8t* pointer;
    const u8t* const end_buffer;
} in_stream;

in_stream* nimbus_in_stream_new(tyran_memory* memory, int max_octets);
int nimbus_in_stream_read_u16t(in_stream* self, u16t* data);
int nimbus_in_stream_read_u32t(in_stream* self, u32t* data);
int nimbus_in_stream_read_string(in_stream* self, char* data, int max_length);

#endif
