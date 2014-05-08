#ifndef nimbus_combines_h
#define nimbus_combines_h

#include <tyranscript/tyran_memory_pool.h>
#include <tyran_engine/state/component.h>

struct tyran_memory;
struct nimbus_combine;
struct nimbus_state;

typedef struct nimbus_combines {
	struct tyran_memory_pool* combines;
} nimbus_combines;

void nimbus_combines_init(nimbus_combines* self, struct tyran_memory* memory, int max_count);
void nimbus_combines_free(nimbus_combines* self);

struct nimbus_combine* nimbus_combines_create(nimbus_combines* self, struct nimbus_state* state);
void nimbus_combines_destroy(nimbus_combines* self, struct nimbus_combine* combine);

#endif
