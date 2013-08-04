#include <tyranscript/debug/tyran_runtime_debug.h>
#include <tyranscript/tyran_object.h>
#include <tyranscript/tyran_memory_pool_iterator.h>
#include <tyranscript/tyran_runtime.h>
#include <tyranscript/tyran_property_iterator.h>
#include <tyranscript/tyran_symbol_table.h>

void check_referencing(tyran_symbol_table* symbol_table, const tyran_object* o)
{
	tyran_property_iterator it;
	tyran_property_iterator_init(&it, o);

	const tyran_value* value;
	tyran_symbol symbol;

	while (tyran_property_iterator_next(&it, &symbol, &value)) {
		if (tyran_value_is_object_generic(value) && tyran_value_object(value) == o) {
			TYRAN_LOG("referenced by object member '%s'",  tyran_symbol_table_lookup(symbol_table, &symbol));
		}
	}
}

void tyran_runtime_debug_who_is_referencing(struct tyran_runtime* runtime, const struct tyran_object* o)
{
	tyran_memory_pool_iterator it;

	tyran_memory_pool_iterator_init(&it, runtime->object_pool);

	void* object;
	int i=0;
	while (tyran_memory_pool_iterator_next(&it, &object)) {
		const tyran_object* found_object = (tyran_object*) object;
		check_referencing(runtime->symbol_table, found_object);
		i++;
	}
	TYRAN_LOG("Scanned %d objects", i);

	tyran_memory_pool_iterator_free(&it);
}
