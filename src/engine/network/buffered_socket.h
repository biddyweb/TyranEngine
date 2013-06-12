#ifndef nimbus_buffered_socket_h
#define nimbus_buffered_socket_h

struct nimbus_buffered_socket;
struct tyran_memory;
struct nimbus_connecting_socket;


struct nimbus_buffered_socket* nimbus_buffered_socket_new(struct tyran_memory* memory, struct nimbus_connecting_socket* socket);
void nimbus_buffered_socket_run(void* _self);

#endif
