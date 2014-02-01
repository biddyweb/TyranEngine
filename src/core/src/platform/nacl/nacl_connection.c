#include "nacl_connection.h"

#include <tyranscript/tyran_log.h>

#include <tyran_core/platform/nacl/nacl.h>

static void _on_update(void* self)
{
}

static void on_connected(void* user_data, int32_t octets_read)
{
}


static void connect(nimbus_nacl_connection* self)
{
	const char* string = "ws://spelmotor.com:8080";
	struct PP_Var property_var_string = g_nacl.var->VarFromUtf8(string, tyran_strlen(string));

	struct PP_CompletionCallback callback;
	callback.user_data = self;
	callback.func = on_connected;
	callback.flags = 0;

	int32_t result = self->websocket_interface->Connect(self->socket, property_var_string, 0, 0, callback);
	TYRAN_LOG("connect result %d", result);
	g_nacl.var->Release(property_var_string);
}

static PP_Resource create_socket(nimbus_nacl_connection* self)
{
	PP_Resource socket = self->websocket_interface->Create(g_nacl.module_instance);

	return socket;
}

void nimbus_nacl_connection_init(void* _self, tyran_memory* memory)
{
	nimbus_nacl_connection* self = _self;
	nimbus_update_init(&self->update, memory, _on_update, self, "nacl_connection");

	self->websocket_interface = (PPB_WebSocket*) g_nacl.get_browser(PPB_WEBSOCKET_INTERFACE);

	self->socket = create_socket(self);

	connect(self);
}
