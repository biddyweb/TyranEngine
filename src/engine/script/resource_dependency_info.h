#ifndef nimbus_resource_dependency_info_h
#define nimbus_resource_dependency_info_h

#include <tyran_engine/resource/id.h>

typedef struct nimbus_resource_dependency {
	nimbus_resource_id resource_id;
	struct tyran_value* target;
	struct tyran_object* inherit_object;
	tyran_boolean is_inherit;
} nimbus_resource_dependency;

typedef struct nimbus_resource_dependency_info {
	nimbus_resource_id resource_id;
	nimbus_resource_dependency resource_dependencies[32];
	int resource_dependencies_count;
	struct tyran_object* target;
} nimbus_resource_dependency_info;

void nimbus_resource_dependency_info_init(nimbus_resource_dependency_info* self, nimbus_resource_id resource_id, struct tyran_object* target);
void nimbus_resource_dependency_info_add_resource(nimbus_resource_dependency_info* self, struct tyran_value* target, nimbus_resource_id resource_id);
void nimbus_resource_dependency_info_inherit(nimbus_resource_dependency_info* self, struct tyran_object* target, nimbus_resource_id resource_id);
void nimbus_resource_dependency_info_delete_at(nimbus_resource_dependency_info* info, int index);
tyran_boolean nimbus_resource_dependency_info_is_satisfied(nimbus_resource_dependency_info* info);
void nimbus_resource_dependency_info_free(nimbus_resource_dependency_info* self);
#endif
