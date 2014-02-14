#ifndef nimbus_component_h
#define nimbus_component_h

#include <tyranscript/tyran_types.h>

struct nimbus_component_definition;
struct nimbus_combine;

typedef struct nimbus_component {
	const struct nimbus_component_definition* component_definition;
	struct nimbus_combine* parent_combine;
	u8t* component_data;
} nimbus_component;

void nimbus_component_init(nimbus_component* self, struct nimbus_combine* owner, const struct nimbus_component_definition* definition, u8t* data, size_t data_octet_size);
void nimbus_component_move(nimbus_component* self, u8t* new_data, size_t data_octet_size);

#endif
