#include <tyran_engine/module/nimbus_module.h>
#include <tyranscript/tyran_memory.h>

void* nimbus_module_create(nimbus_module* self, struct tyran_memory* memory)
{
	void* struct_memory = TYRAN_MEMORY_ALLOC(memory, self->octet_size, self->name);

	self->init_func(struct_memory, memory);

	return struct_memory;
}

struct nimbus_update* nimbus_module_get_update(nimbus_module* self, void* struct_memory) {
	return (struct nimbus_update*) struct_memory;
}
