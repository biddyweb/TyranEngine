#ifndef resource_handler_h
#define resource_handler_h

#include <tyran_engine/resource/id.h>

struct tyran_memory;

typedef struct nimbus_resource_info {
	nimbus_resource_id resource_id;
	const char* filename;
} nimbus_resource_info;

typedef struct nimbus_resource_handler {
	nimbus_resource_info* resource_infos;
	int resource_infos_count;
	int resource_infos_max_count;
	struct tyran_memory* memory;
} nimbus_resource_handler;

nimbus_resource_handler* nimbus_resource_handler_new(struct tyran_memory* memory);
nimbus_resource_id nimbus_resource_handler_add(nimbus_resource_handler* self, const char* name);
const char* nimbus_resource_handler_id_to_name(nimbus_resource_handler* self, nimbus_resource_id id);

#endif
