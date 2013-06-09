#ifndef nimbus_base_connecting_socket_h
#define nimbus_base_connecting_socket_h

typedef struct nimbus_engine_connecting_socket {
	int socket_handle;
} nimbus_engine_connecting_socket;

struct tyran_memory;

nimbus_engine_connecting_socket* nimbus_engine_connecting_socket_new(struct tyran_memory* memory, const char* name, int port);

#endif
