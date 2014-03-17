#ifndef nimbus_component_h
#define nimbus_component_h

#include <tyranscript/tyran_types.h>
#include <tyranscript/tyran_symbol.h>

struct nimbus_component_definition;
struct nimbus_combine;
struct nimbus_event_component_header;

typedef struct nimbus_component {
	const struct nimbus_component_definition* component_definition;
	struct nimbus_combine* parent_combine;
	struct nimbus_event_component_header* component_data;
	tyran_symbol component_name;
} nimbus_component;

void nimbus_component_init(nimbus_component* self, struct nimbus_combine* owner, tyran_symbol component_name, const struct nimbus_component_definition* definition, struct nimbus_event_component_header* data, size_t data_octet_size);
void nimbus_component_move(nimbus_component* self, u8t* new_data, size_t data_octet_size);

#endif
