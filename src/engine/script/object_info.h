#ifndef nimbus_object_info_h
#define nimbus_object_info_h

typedef struct nimbus_object_info {
	int instance_id;
	int track_index;
	tyran_boolean is_module_resource;
	tyran_boolean is_spawned_combine;
} nimbus_object_info;

#endif
