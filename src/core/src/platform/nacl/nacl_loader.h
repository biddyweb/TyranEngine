#ifndef nimbus_nacl_loader_h
#define nimbus_nacl_loader_h

#include <ppapi/c/ppb_url_loader.h>
#include <ppapi/c/ppb_url_request_info.h>
#include <tyran_core/update/update.h>
#include <tyranscript/tyran_memory_pool.h>
#include <tyran_engine/resource/id.h>

struct tyran_memory;
struct tyran_memory_pool;

typedef struct nimbus_nacl_loader {
	nimbus_update update_object;

	PPB_URLRequestInfo* url_request_info;
	PPB_URLLoader* url_loader;
	struct tyran_memory_pool* resource_progress_pool;

	int in_buffer_size;
	u8t* in_buffer;
} nimbus_nacl_loader;

void nimbus_nacl_loader_init(void* self, struct tyran_memory* memory);

#endif
