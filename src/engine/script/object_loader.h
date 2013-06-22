#ifndef nimbus_object_loader_h
#define nimbus_object_loader_h

#include "../../core/src/base/update/nimbus_update.h"

struct tyran_mocha_api;
struct tyran_value;

typedef struct nimbus_object_loader {
	struct tyran_mocha_api* mocha;
	struct tyran_value* context;
	nimbus_update update;
	u8t* script_buffer;
	int script_buffer_size;
} nimbus_object_loader;

void nimbus_object_loader_init(nimbus_object_loader* self, struct tyran_memory* memory, struct tyran_mocha_api* mocha, struct tyran_value* context);
void nimbus_object_loader_free(nimbus_object_loader* self);
#endif
