#ifndef nimbus_resource_updated_h
#define nimbus_resource_updated_h

#include "../resource/resource_id.h"

extern const u8t NIMBUS_EVENT_RESOURCE_UPDATED;

typedef struct nimbus_resource_updated {
	nimbus_resource_id resource_id;
	int payload_size;
} nimbus_resource_updated;


#endif
