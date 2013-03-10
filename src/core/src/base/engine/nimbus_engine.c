#include "nimbus_engine.h"

TYRAN_RUNTIME_CALL_FUNC(nimbus_engine_load_library)
{
	return 0;
}

nimbus_engine* nimbus_engine_new(tyran_memory* memory)
{
	nimbus_engine* self = TYRAN_MEMORY_CALLOC_TYPE(memory, nimbus_engine);
	// self->listener = nimbus_event_listener_new(memory, self);
	//nimbus_event_listener_listen(self->listener, 1, );

	tyran_mocha_api_new(&self->mocha_api, 1024);

	tyran_value* global = tyran_runtime_context(self->mocha_api.default_runtime);
	tyran_mocha_api_add_function(&self->mocha_api, global, "load_library", nimbus_engine_load_library);

	return self;
}

tyran_boolean nimbus_engine_vertical_refresh(nimbus_engine* engine)
{
	return TYRAN_FALSE;
}
