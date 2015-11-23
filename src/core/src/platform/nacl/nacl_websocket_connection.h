#ifndef nimbus_nacl_connection_h
#define nimbus_nacl_connection_h

#include <tyran_core/update/update.h>

#include <ppapi/c/ppb_websocket.h>

struct tyran_memory;

typedef struct nimbus_nacl_connection {
	nimbus_update update;
	PPB_WebSocket* websocket_interface;
	PP_Resource socket;
} nimbus_nacl_connection;

void nimbus_nacl_connection_init(void* self, struct tyran_memory* memory);

#endif
