#include "resource_dependency_info.h"
#include <tyranscript/tyran_value.h>

void nimbus_resource_dependency_info_init(nimbus_resource_dependency_info* self, nimbus_resource_id resource_id, struct tyran_object* target)
{
	self->target = target;
	self->resource_id = resource_id;
	self->inherit_resource_id = 0;
	self->resource_dependencies_count = 0;
}

void nimbus_resource_dependency_info_free(nimbus_resource_dependency_info* self)
{

}

void nimbus_resource_dependency_info_add_resource(nimbus_resource_dependency_info* self, tyran_value* target, nimbus_resource_id resource_id)
{
	nimbus_resource_dependency* dependency = &self->resource_dependencies[self->resource_dependencies_count];
	self->resource_dependencies_count++;
	dependency->resource_id = resource_id;
	dependency->target = target;
}

void nimbus_resource_dependency_info_delete_at(nimbus_resource_dependency_info* info, int index)
{
	tyran_memmove_type(nimbus_resource_dependency, &info->resource_dependencies[index], &info->resource_dependencies[index + 1], info->resource_dependencies_count - index - 1);
	info->resource_dependencies_count--;
}

void nimbus_resource_dependency_info_inherit(nimbus_resource_dependency_info* self, nimbus_resource_id resource_id)
{
	self->inherit_resource_id = resource_id;
}

tyran_boolean nimbus_resource_dependency_info_is_satisfied(nimbus_resource_dependency_info* self)
{
	return (self->inherit_resource_id == 0) && (self->resource_dependencies_count == 0);
}
