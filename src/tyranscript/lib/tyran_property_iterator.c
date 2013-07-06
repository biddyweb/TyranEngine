#include <tyranscript/tyran_property_iterator.h>
#include <tyranscript/tyran_object.h>

void tyran_property_iterator_init(tyran_property_iterator* self, tyran_object* object)
{
	TYRAN_LOG("iterator %p init", self);
	self->property_index = 0;
	self->object = object;
	self->visited_symbols_count = 0;
	self->visited_symbols_max_count = 64;
}

void tyran_property_iterator_free(tyran_property_iterator* self)
{
	TYRAN_LOG("iterator %p free", self);
}

static tyran_boolean have_visited_symbol(tyran_property_iterator* self, tyran_symbol symbol)
{
	for (int i=0; i<self->visited_symbols_count; ++i) {
		if (self->visited_symbols[i].hash == symbol.hash) {
			TYRAN_LOG("Found match:%d == %d", self->visited_symbols[i].hash, symbol.hash);
			return TYRAN_TRUE;
		}
	}

	return TYRAN_FALSE;
}

static void add_symbol(tyran_property_iterator* self, tyran_symbol symbol)
{
	TYRAN_ASSERT(self->visited_symbols_count < self->visited_symbols_max_count, "Passed too many symbols");
	self->visited_symbols[self->visited_symbols_count++] = symbol;
}

tyran_boolean tyran_property_iterator_next(tyran_property_iterator* self, tyran_symbol* symbol, tyran_value** value)
{
	if (self->property_index >= self->object->property_count) {
		TYRAN_LOG("No more properties on this object");
		tyran_object* parent = self->object->prototype;
		TYRAN_ASSERT(parent != self->object, "Prototype can not be self!");
		if (!parent) {
			TYRAN_LOG("property iterator end");
			return TYRAN_FALSE;
		} else {
			self->property_index = 0;
			self->object = parent;
			TYRAN_LOG("going to parent");
			return tyran_property_iterator_next(self, symbol, value);
		}
	} else {
		tyran_object_property* property = &self->object->properties[self->property_index++];
		if (have_visited_symbol(self, property->symbol)) {
			TYRAN_LOG("we already been to this one, skip to next");
			return tyran_property_iterator_next(self, symbol, value);
		} else {
			TYRAN_LOG("next property");
			*symbol = property->symbol;
			*value = &property->value;
			add_symbol(self, property->symbol);
		}
	}

	return TYRAN_TRUE;
}
