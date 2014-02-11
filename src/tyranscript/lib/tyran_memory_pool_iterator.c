#include <tyranscript/tyran_memory_pool_iterator.h>
#include <tyranscript/tyran_memory_pool.h>

void tyran_memory_pool_iterator_init(tyran_memory_pool_iterator* self, tyran_memory_pool* pool)
{
	self->index = 0;
	self->pool = pool;
}


tyran_boolean tyran_memory_pool_iterator_next(tyran_memory_pool_iterator* self, void** object)
{
	if (self->index == self->pool->max_count) {
		return TYRAN_FALSE;
	}

	for (; self->index < self->pool->max_count;) {
		const u8t* raw = (const u8t*)self->pool->memory + (sizeof(tyran_memory_pool_entry) + self->pool->struct_size) * self->index++;
		const tyran_memory_pool_entry* e = (const tyran_memory_pool_entry*) raw;
		if (e->allocated) {
			*object = (void*)(raw + sizeof(tyran_memory_pool_entry));
			return TYRAN_TRUE;
		}
	}

	return TYRAN_FALSE;
}

void tyran_memory_pool_iterator_free(tyran_memory_pool_iterator* self)
{
	self->pool = 0;
}
