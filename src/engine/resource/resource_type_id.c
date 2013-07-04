#include "resource_type_id.h"

#include "resource_id.h"

nimbus_resource_type_id nimbus_resource_type_id_from_string(const char* name)
{
	return nimbus_resource_id_from_string(name);
}
