#ifndef nimbus_base_connecting_socket_h
#define nimbus_base_connecting_socket_h

#include <tyranscript/tyran_types.h>

typedef struct nimbus_connecting_socket {
	int socket_handle;
} nimbus_connecting_socket;

struct tyran_memory;

void nimbus_connecting_socket_init(nimbus_connecting_socket* self, const char* name, int port);
void nimbus_connecting_socket_write(nimbus_connecting_socket* self, const u8t* data, int length);
int nimbus_connecting_socket_read(nimbus_connecting_socket* self, u8t* data, int max_length);
void nimbus_connecting_socket_close(nimbus_connecting_socket* self);

#endif
