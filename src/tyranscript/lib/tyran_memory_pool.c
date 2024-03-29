#include <tyranscript/tyran_memory_pool.h>
#include <tyranscript/tyran_memory.h>
#include <tyranscript/tyran_log.h>

void tyran_memory_pool_initialize_entries(tyran_memory_pool* pool)
{
	u8t* m = pool->memory;
	int total_size = (int) (pool->struct_size + sizeof(tyran_memory_pool_entry));
	tyran_memory_pool_entry* previous = 0;
	for (int i = 0; i < pool->max_count; ++i) {
		tyran_memory_pool_entry* e = (tyran_memory_pool_entry*) m;
		if (i == 0) {
			pool->first_free = e;
		}
		e->allocated = TYRAN_FALSE;
		e->line = -1;
		e->file = 0;
		e->next_free = 0;
		e->pool = pool;
		if (previous) {
			previous->next_free = e;
		}
		previous = e;
		m += total_size;
	}
}

tyran_memory_pool* tyran_memory_pool_construct(tyran_memory* memory, size_t struct_size, size_t count, const char* type)
{
	tyran_memory_pool* pool = TYRAN_MEMORY_ALLOC(memory, sizeof(tyran_memory_pool), "Memory pool");
	pool->memory = TYRAN_MEMORY_ALLOC(memory, (sizeof(tyran_memory_pool_entry) + struct_size) * count, "Memory pool entries");
	pool->struct_size = struct_size;
	pool->type_string = type;
	pool->max_count = (int) count;
	pool->count = 0;
	tyran_memory_pool_initialize_entries(pool);
	return pool;
}

void* tyran_memory_pool_alloc(tyran_memory_pool* pool)
{
	TYRAN_ASSERT(pool->count < pool->max_count, "Out of memory in pool (%d)", pool->count);
	tyran_memory_pool_entry* e = pool->first_free;
	TYRAN_ASSERT(e, "first free is null");
	pool->first_free = e->next_free;
	pool->count++;
	u8t* m = (u8t*) e;
	u8t* p = m + sizeof(tyran_memory_pool_entry);
	e->allocated = TYRAN_TRUE;
	// TYRAN_ASSERT(((tyran_pointer_to_number)p) % alignment == 0, "alignment error");
	// TYRAN_LOG("Allocating from memory pool '%s' (%zu) -> %p (count:%zu)", pool->type_string, pool->struct_size, m,
	// pool->count);
	return p;
}

void* tyran_memory_pool_alloc_debug(tyran_memory_pool* pool, const char* type_name, size_t struct_size)
{
	TYRAN_ASSERT(tyran_strcmp(pool->type_string, type_name) == 0, "Type name mismatch. Expected:%s received %s",
				 pool->type_string, type_name);
	TYRAN_ASSERT(pool->struct_size == struct_size, "Struct size mismatch. Expected %zu, received %zu", pool->struct_size,
				 struct_size);

	return tyran_memory_pool_alloc(pool);
}

void* tyran_memory_pool_calloc(tyran_memory_pool* pool)
{
	void* p = tyran_memory_pool_alloc(pool);
	TYRAN_ASSERT(p, "not null");
	tyran_mem_clear(p, pool->struct_size);
	return p;
}

void* tyran_memory_pool_calloc_debug(tyran_memory_pool* pool, const char* type_name, size_t struct_size)
{
	TYRAN_ASSERT(tyran_strcmp(pool->type_string, type_name) == 0, "Type name mismatch. Expected:%s received %s",
				 pool->type_string, type_name);
	TYRAN_ASSERT(pool->struct_size == struct_size, "Struct size mismatch. Expected %zu, received %zu", pool->struct_size,
				 struct_size);

	return tyran_memory_pool_calloc(pool);
}

void tyran_memory_pool_free(void* p)
{
	u8t* m = p;
	tyran_memory_pool_entry* e = (tyran_memory_pool_entry*) (m - sizeof(tyran_memory_pool_entry));
	e->allocated = TYRAN_FALSE;

	tyran_memory_pool* pool = e->pool;

	pool->count--;

	// TYRAN_LOG("Freeing from memory pool '%s' (%zu) -> %p (count:%zu)", pool->type_string, pool->struct_size, m, pool->count);
	e->next_free = e->pool->first_free;
	pool->first_free = e;
}

char* tyran_str_dup(tyran_memory* memory, const char* str)
{
	int size = tyran_strlen(str);
	char* mem = TYRAN_MALLOC_NO_POOL_TYPE_COUNT(memory, char, size + 1);

	tyran_strncpy(mem, size + 1, str, size);

	return mem;
}
