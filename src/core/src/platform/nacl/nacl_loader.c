#include "nacl_loader.h"

#include <tyran_core/platform/nacl/nacl.h>

#include "../../base/buffer/octet_buffer.h"

#include <tyranscript/tyran_clib.h>

#include <tyran_engine/event/resource_load.h>
#include <tyran_engine/resource/type_id.h>
#include <tyran_engine/event/resource_updated.h>

#include <ppapi/c/pp_errors.h>

typedef struct resource_progress {
	PP_Resource url_loader_instance;
	PP_Resource request_info_instance;
	nimbus_nacl_loader* loader;
	nimbus_resource_id resource_id;
	nimbus_resource_type_id resource_type_id;
	nimbus_octet_buffer buffer;
	nimbus_octet_buffer in_buffer;
} resource_progress;

static void set_string_property(PPB_URLRequestInfo* interface, PP_Resource request, PP_URLRequestProperty property, const char* string)
{
	TYRAN_LOG("set_string_property '%s'", string);
	struct PP_Var property_var_string = g_nacl.var->VarFromUtf8(string, tyran_strlen(string));
	interface->SetProperty(request, property, property_var_string);
	g_nacl.var->Release(property_var_string);
}

static nimbus_resource_type_id generic_resource_type_to_extension_type(nimbus_resource_type_id resource_type_id)
{
	const char* converted = "oec";

	if (resource_type_id == nimbus_resource_type_id_from_string("image")) {
		converted = "png";
	} else if (resource_type_id == nimbus_resource_type_id_from_string("vertex")) {
		converted = "vsh";
	} else if (resource_type_id == nimbus_resource_type_id_from_string("fragment")) {
		converted = "fsh";
	} else if (resource_type_id == nimbus_resource_type_id_from_string("skeleton")) {
		converted = "skeleton";
	} else if (resource_type_id == nimbus_resource_type_id_from_string("script")) {
		converted = "oes";
	}

	return nimbus_resource_type_id_from_string(converted);
}

static void extension_from_resource_type(char* extension, int extension_size, nimbus_resource_type_id resource_type_id)
{
	tyran_strcpy(extension, extension_size, nimbus_resource_id_debug_name(resource_type_id));
}

static PP_Resource create_request_info_instance(nimbus_nacl_loader* self, nimbus_resource_id resource_id, nimbus_resource_type_id resource_type_id)
{
	TYRAN_LOG("create_request_info_instance");
	PP_Resource request = self->url_request_info->Create(g_nacl.module_instance);

	const char* resource_name = nimbus_resource_id_debug_name(resource_id);

	const int extension_size = 512;
	char extension[extension_size];

	extension_from_resource_type(extension, extension_size, resource_type_id);

	const int uri_size = 512;
	char uri[uri_size];
	tyran_snprintf(uri, uri_size, "%s.%s", resource_name, extension);

	set_string_property(self->url_request_info, request, PP_URLREQUESTPROPERTY_URL, uri);
	set_string_property(self->url_request_info, request, PP_URLREQUESTPROPERTY_METHOD, "GET");
	TYRAN_LOG("Created request info '%s'", uri);

	return request;
}

static void send_resource_update(nimbus_event_write_stream* out_event_stream, nimbus_resource_id resource_id, nimbus_resource_type_id resource_type_id, const u8t* payload, int payload_size)
{
	TYRAN_LOG("Send Resource Update. Payload size:%d", payload_size);
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

static resource_progress* create_progress(nimbus_nacl_loader* self, nimbus_resource_id resource_id, nimbus_resource_type_id resource_type_id, PP_Resource url_loader_instance, PP_Resource request_info_instance)
{
	TYRAN_LOG("Creating progress");
	resource_progress* progress = TYRAN_CALLOC_TYPE(self->resource_progress_pool, resource_progress);
	nimbus_octet_buffer_init(&progress->buffer, self->memory, 64 * 1024);
	nimbus_octet_buffer_init(&progress->in_buffer, self->memory, 128 * 1024);
	progress->request_info_instance = request_info_instance;
	progress->url_loader_instance = url_loader_instance;
	progress->resource_id = resource_id;
	progress->resource_type_id = resource_type_id;
	progress->loader = self;

	return progress;
}

static void free_progress(resource_progress* progress)
{
	TYRAN_LOG("Freeing progress");
	nimbus_octet_buffer_free(&progress->buffer);
	TYRAN_LOG("Freeing progress2");
	nimbus_octet_buffer_free(&progress->in_buffer);
	TYRAN_LOG("Freeing progress3");

	// Free request info
	// Free url loader instance
	progress->loader = 0;
	TYRAN_LOG("Freeing progress4");

	TYRAN_MALLOC_FREE(progress);
	TYRAN_LOG("Freeing progress5");
}


static void resource_data_complete(nimbus_nacl_loader* self, resource_progress* progress)
{
	TYRAN_LOG("data_complete");
	send_resource_update(&self->update_object.event_write_stream, progress->resource_id, progress->resource_type_id, progress->buffer.octets, progress->buffer.size);
	TYRAN_LOG("Resource sent");
	free_progress(progress);
}

static void add_resource_data_to_buffer(resource_progress* progress, int data_size)
{
	nimbus_octet_buffer_add(&progress->buffer, progress->in_buffer.octets, data_size);
}

static void on_resource_data_chunk_callback(void* user_data, int32_t octets_read);

static void continue_reading(nimbus_nacl_loader* self, resource_progress* progress)
{
	struct PP_CompletionCallback callback;
	callback.func = on_resource_data_chunk_callback;
	callback.user_data = progress;
	callback.flags = 0;

	int32_t result;
	do {
		result = self->url_loader->ReadResponseBody(progress->url_loader_instance, progress->in_buffer.octets, progress->in_buffer.max_size, callback);
		if (result > 0) {
			add_resource_data_to_buffer(progress, result);
		} else if (result != PP_OK_COMPLETIONPENDING) {
			on_resource_data_chunk_callback(callback.user_data, result);
		}
	} while (result > 0);
}

static void on_resource_data_chunk_callback(void* user_data, int32_t result)
{
	TYRAN_LOG("on_resource_data_chunk_callback:%d", result);
	resource_progress* progress = user_data;
	nimbus_nacl_loader* self = progress->loader;

	if (result == PP_OK) {
		resource_data_complete(self, progress);
	} else if (result > 0) {
		add_resource_data_to_buffer(progress, result);
		continue_reading(self, progress);
	} else {
		TYRAN_ERROR("Error in callback:%d", result);
	}
}

static void on_resource_opened(void* user_data, int32_t result)
{
	resource_progress* progress = user_data;
	nimbus_nacl_loader* self = progress->loader;

	TYRAN_LOG("We opened resource:%d", progress->resource_id);
	continue_reading(self, progress);
}

static void request_open(nimbus_nacl_loader* self, nimbus_resource_id resource_id, nimbus_resource_type_id resource_type_id, PP_Resource url_loader_instance, PP_Resource request_info_instance)
{
	struct PP_CompletionCallback callback;
	TYRAN_LOG("request_open");
	resource_progress* progress = create_progress(self, resource_id, resource_type_id, url_loader_instance, request_info_instance);
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

static void on_resource_request(nimbus_nacl_loader* self, nimbus_resource_id resource_id, nimbus_resource_type_id resource_type_id)
{
	nimbus_resource_type_id extension_type_id = generic_resource_type_to_extension_type(resource_type_id);
	TYRAN_LOG("on_resource_request %d", resource_id);
	PP_Resource request_info_instance = create_request_info_instance(self, resource_id, extension_type_id);
	PP_Resource url_loader_instance = create_url_loader_instance(self);

	request_open(self, resource_id, extension_type_id, url_loader_instance, request_info_instance);
}

static void _on_resource_request(void* _self, struct nimbus_event_read_stream* stream)
{
	nimbus_nacl_loader* self = (nimbus_nacl_loader*) _self;

	nimbus_resource_load resource_load;

	nimbus_event_stream_read_type(stream, resource_load);
	TYRAN_LOG("Detected a resource id request: %d", resource_load.resource_id);
	on_resource_request(self, resource_load.resource_id, resource_load.resource_type_id);
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
	self->memory = memory;
	const int largest_resource_file = 1024 * 1024;

	nimbus_update_init_ex(&self->update_object, memory, _on_update, self, largest_resource_file, "nacl_loader");
	nimbus_event_listener_listen(&self->update_object.event_listener, NIMBUS_EVENT_RESOURCE_LOAD, _on_resource_request);
}
