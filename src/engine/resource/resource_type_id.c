#include <tyran_engine/resource/type_id.h>

#include <tyran_engine/resource/id.h>

#include <tyranscript/tyran_log.h>

nimbus_resource_type_id nimbus_resource_type_id_from_string(const char* name)
{
	nimbus_resource_type_id type_id = nimbus_resource_id_from_string(name);
	TYRAN_LOG("Name '%s' -> %d", name, type_id);

	return type_id;
}
