#ifndef tyran_memory_pool_iterator_h

#include <tyranscript/tyran_types.h>

struct tyran_memory_pool;
struct tyran_memory_pool_entry;

typedef struct tyran_memory_pool_iterator {
	int index;
	struct tyran_memory_pool* pool;
} tyran_memory_pool_iterator;

void tyran_memory_pool_iterator_init(struct tyran_memory_pool_iterator* self, struct tyran_memory_pool* pool);
tyran_boolean tyran_memory_pool_iterator_next(struct tyran_memory_pool_iterator* self, void** object);
void tyran_memory_pool_iterator_free(struct tyran_memory_pool_iterator* self);

#endif
