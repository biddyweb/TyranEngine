#include <tyran_engine/event/module_resource_updated.h>
#include <tyran_engine/event/resource_updated.h>
#include <tyran_engine/resource/type_id.h>
#include <tyranscript/tyran_log.h>


void nimbus_event_module_resource_updated_send(struct nimbus_event_write_stream* stream, nimbus_resource_id id, module_resource_index index)
{
	nimbus_resource_type_id type_id = nimbus_resource_type_id_from_string("module_resource");

	nimbus_resource_updated_send(stream, id, type_id, &index, sizeof(index));
}
