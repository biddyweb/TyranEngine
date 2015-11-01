#ifndef nimbus_resource_id_h
#define nimbus_resource_id_h

#include <tyranscript/tyran_types.h>

typedef u32t nimbus_resource_id;

const char* nimbus_resource_id_debug_name(nimbus_resource_id id);
nimbus_resource_id nimbus_resource_id_from_string(const char* name);

#endif
