#ifndef nimbus_extra_reference_h
#define nimubs_extra_reference_h

typedef struct nimubs_extra_reference {
	nimbus_resource_id resource_id;
	tyran_symbol component_name;
	union {
		nimbus_component** component;
	} pointer;
	bool is_loaded;
} nimbus_extra_reference;

#endif
