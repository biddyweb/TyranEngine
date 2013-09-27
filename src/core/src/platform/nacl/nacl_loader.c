#include "nacl_loader.h"

#include <tyranscript/tyran_clib.h>
#include <tyran_core/platform/nacl/nacl.h>
#include <tyran_engine/event/resource_load.h>
#include <tyran_engine/resource/type_id.h>
#include <tyran_engine/event/resource_updated.h>

#include <ppapi/c/pp_errors.h>

typedef struct resource_progress {
	PP_Resource url_loader_instance;
	PP_Resource request_info_instance;
	nimbus_nacl_loader* loader;
	nimbus_resource_id resource_id;
} resource_progress;

static void set_string_property(PPB_URLRequestInfo* interface, PP_Resource request, PP_URLRequestProperty property, const char* string)
{
	TYRAN_LOG("set_string_property '%s'", string);
	struct PP_Var property_var_string = g_nacl.var->VarFromUtf8(string, tyran_strlen(string));
	interface->SetProperty(request, property, property_var_string);
	g_nacl.var->Release(property_var_string);
}

static PP_Resource create_request_info_instance(nimbus_nacl_loader* self, nimbus_resource_id resource_id)
{
	TYRAN_LOG("create_request_info_instance");
	PP_Resource request = self->url_request_info->Create(g_nacl.module_instance);

	const char* uri = nimbus_resource_id_debug_name(resource_id);
	set_string_property(self->url_request_info, request, PP_URLREQUESTPROPERTY_URL, uri);
	set_string_property(self->url_request_info, request, PP_URLREQUESTPROPERTY_METHOD, "GET");
	TYRAN_LOG("Created request info '%s'", uri);

	return request;
}

static void send_resource_update(nimbus_event_write_stream* out_event_stream, nimbus_resource_id resource_id, nimbus_resource_type_id resource_type_id, const u8t* payload, int payload_size)
{
	TYRAN_LOG("Send Resource Update");
	nimbus_resource_updated resource_updated;
	resource_updated.resource_id = resource_id;
	resource_updated.resource_type_id = resource_type_id;
	resource_updated.payload_size = payload_size;

	nimbus_event_stream_write_event_header(out_event_stream, NIMBUS_EVENT_RESOURCE_UPDATED);
	nimbus_event_stream_write_type(out_event_stream, resource_updated);
	nimbus_event_stream_write_octets(out_event_stream, payload, payload_size);
	nimbus_event_stream_write_event_end(out_event_stream);
	TYRAN_LOG("SenTTT Resource Update");

}

static void on_resource_data(void* user_data, int32_t octets_read)
{
	TYRAN_LOG("on_resource_data octets:%d", octets_read);
	resource_progress* progress = user_data;
	nimbus_resource_type_id resource_type_id = nimbus_resource_type_id_from_string("oes");
	nimbus_nacl_loader* self = progress->loader;
	send_resource_update(&self->update_object.event_write_stream, progress->resource_id, resource_type_id, self->in_buffer, octets_read);
}

static void on_resource_opened(void* user_data, int32_t result)
{
	resource_progress* progress = user_data;
	nimbus_nacl_loader* self = progress->loader;

	TYRAN_LOG("We opened resource:%d", progress->resource_id);

	struct PP_CompletionCallback callback;
	callback.func = on_resource_data;
	callback.user_data = progress;
	callback.flags = 0;

	int32_t octets_read = self->url_loader->ReadResponseBody(progress->url_loader_instance, self->in_buffer, self->in_buffer_size, callback);
	if (octets_read >= 0) {
		on_resource_data(progress, octets_read);
	} else if (octets_read != PP_OK_COMPLETIONPENDING) {
		TYRAN_ERROR("ResponseBody Error:%d", octets_read);
	}
}

static void request_open(nimbus_nacl_loader* self, nimbus_resource_id resource_id, PP_Resource url_loader_instance, PP_Resource request_info_instance)
{
	struct PP_CompletionCallback callback;
	TYRAN_LOG("request_open");
	resource_progress* progress = TYRAN_CALLOC_TYPE(self->resource_progress_pool, resource_progress);
	progress->request_info_instance = request_info_instance;
	progress->url_loader_instance = url_loader_instance;
	progress->resource_id = resource_id;
	progress->loader = self;
	callback.func = on_resource_opened;
	callback.user_data = progress;
	callback.flags = 0;
	self->url_loader->Open(url_loader_instance, request_info_instance, callback);
}

static PP_Resource create_url_loader_instance(nimbus_nacl_loader* self)
{
	TYRAN_LOG("Create url loader instance");
	PP_Resource url_loader = self->url_loader->Create(g_nacl.module_instance);
	return url_loader;
}

static void on_resource_request(nimbus_nacl_loader* self, nimbus_resource_id resource_id)
{
	TYRAN_LOG("on_resource_request %d", resource_id);
	PP_Resource request_info_instance = create_request_info_instance(self, resource_id);
	PP_Resource url_loader_instance = create_url_loader_instance(self);

	request_open(self, resource_id, url_loader_instance, request_info_instance);
}

static void _on_resource_request(void* _self, struct nimbus_event_read_stream* stream)
{
	nimbus_nacl_loader* self = (nimbus_nacl_loader*) _self;

	nimbus_resource_id resource_id;

	nimbus_event_stream_read_type(stream, resource_id);
	TYRAN_LOG("Detected a resource id request: %d", resource_id);

	on_resource_request(self, resource_id);
}

static void _on_update(void* self)
{
}

void nimbus_nacl_loader_init(void* _self, tyran_memory* memory)
{
	nimbus_nacl_loader* self = _self;
	self->url_request_info = (PPB_URLRequestInfo*) g_nacl.get_browser(PPB_URLREQUESTINFO_INTERFACE);
	self->url_loader = (PPB_URLLoader*) g_nacl.get_browser(PPB_URLLOADER_INTERFACE);
	self->resource_progress_pool = TYRAN_MEMORY_POOL_CONSTRUCT(memory, resource_progress, 256);
	const int largest_resource_file = 1024 * 1024;

	nimbus_update_init_ex(&self->update_object, memory, _on_update, self, largest_resource_file, "nacl_loader");
	nimbus_event_listener_listen(&self->update_object.event_listener, NIMBUS_EVENT_RESOURCE_LOAD, _on_resource_request);

	self->in_buffer_size = 8 * 1024;
	self->in_buffer = TYRAN_MEMORY_ALLOC(memory, self->in_buffer_size, "nacl_loader_buffer");
	// Ring buffer must be big enough to hold the largest resource file.
	//nimbus_ring_buffer_init(&self->buffer, memory, largest_resource_file);
	//nimbus_mutex_init(&self->ring_buffer_mutex);
}
