#ifndef nimbus_base_connecting_socket_h
#define nimbus_base_connecting_socket_h

typedef struct nimbus_engine_connecting_socket {
	int socket_handle;
} nimbus_engine_connecting_socket;

struct tyran_memory;

nimbus_engine_connecting_socket* nimbus_engine_connecting_socket_new(struct tyran_memory* memory, const char* name, int port);
void nimbus_engine_connecting_socket_write(nimbus_engine_connecting_socket* self, const u8t* data, int length);
int nimbus_engine_connecting_socket_read(nimbus_engine_connecting_socket* self, u8t* data, int max_length);
void nimbus_engine_connecting_socket_close(nimbus_engine_connecting_socket* self);

#endif
