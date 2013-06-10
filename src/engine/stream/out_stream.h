#ifndef nimbus_engine_out_stream_h
#define nimbus_engine_out_stream_h

typedef struct out_stream {
    u8t* buffer;
    u8t* pointer;
    const u8t* end_buffer;
} out_stream;

void nimbus_out_stream_new(tyran_memory* memory, int max_octets);
void nimbus_out_stream_free(nimbus_out_stream* self);

void nimbus_out_stream_write_u16t(out_stream* self, u16t data);
void nimbus_out_stream_write_u32t(out_stream* self, u32t data);
void nimbus_out_stream_write_string(out_stream* self, const char* data);
void nimbus_out_stream_info(out_stream* self, u8t** buffer, int* length);

#endif
