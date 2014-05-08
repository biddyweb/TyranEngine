#include "combines.h"
#include <tyran_engine/state/combine.h>

void nimbus_combines_init(nimbus_combines* self, struct tyran_memory* memory, int max_count)
{
	self->combines = TYRAN_MEMORY_POOL_CONSTRUCT(memory, nimbus_combine, max_count);
}

void nimbus_combines_free(nimbus_combines* self)
{
	// destroy_pool(self->combines);
}

nimbus_combine* nimbus_combines_create(nimbus_combines* self, struct nimbus_state* state)
{
	nimbus_combine* combine = TYRAN_CALLOC_TYPE(self->combines, nimbus_combine);
	nimbus_combine_init(combine, state);

	return combine;
}

void nimbus_combines_destroy(nimbus_combines* self, nimbus_combine* combine)
{
	TYRAN_MALLOC_FREE(combine);
}
