/*
 #include <tyran_engine/script/event_to_object.h>

#include <tyranscript/tyran_symbol_table.h>
#include <tyran_core/event/event_stream.h>

#include <tyranscript/tyran_object.h>
#include <tyranscript/tyran_string.h>
#include <tyranscript/tyran_string_object.h>
#include <tyran_engine/event_definition/event_definition.h>
#include "object_info.h"

#include <tyran_engine/script/property_writer.h>

#include <tyran_engine/type/size2.h>
#include <tyran_engine/type/size2i.h>
#include <tyran_engine/type/vector2.h>

#include <tyranscript/tyran_runtime.h>
#include <stdint.h>

#define NIMBUS_OBJECT_TO_EVENT_MEMBER_ALIGN() { const int alignment = 4; d += (alignment - ((intptr_t)(d) % alignment)) % alignment; }


void nimbus_event_to_object_init(nimbus_event_to_object* self, struct tyran_symbol_table* symbol_table, tyran_runtime* runtime)
{
	tyran_symbol_table_add(symbol_table, &self->x_symbol, "x");
	tyran_symbol_table_add(symbol_table, &self->y_symbol, "y");
	tyran_symbol_table_add(symbol_table, &self->z_symbol, "z");
	tyran_symbol_table_add(symbol_table, &self->width_symbol, "width");
	tyran_symbol_table_add(symbol_table, &self->height_symbol, "height");

	self->runtime = runtime;

	nimbus_property_writer_init(&self->writer, runtime, symbol_table);
}

int nimbus_event_to_object_convert(nimbus_event_to_object* self, tyran_object* o, const u8t* base, nimbus_event_definition* e)
{
	nimbus_property_writer* writer = &self->writer;
	const u8t* d;
	tyran_value destination;

	for (int i = 0; i < e->properties_count; ++i) {
		nimbus_event_definition_property* p = &e->properties[i];
		d = base + p->offset_in_struct;
		switch (p->type) {
			case NIMBUS_EVENT_DEFINITION_FLOAT: {
				tyran_value_set_number(destination, *((float*)d));
			}
			break;
			case NIMBUS_EVENT_DEFINITION_BOOLEAN: {
				tyran_value_set_boolean(destination, *((tyran_boolean*)d));
			}
			break;
			case NIMBUS_EVENT_DEFINITION_INTEGER: {
				tyran_value_set_number(destination, *((int*)d));
			}
			break;
			case NIMBUS_EVENT_DEFINITION_SYMBOL: {
				tyran_value_set_symbol(destination, *((tyran_symbol*)d));
			}
			break;
			case NIMBUS_EVENT_DEFINITION_STRING: {
				int char_count = *(int*) d;
				d += sizeof(int);
				NIMBUS_OBJECT_TO_EVENT_MEMBER_ALIGN();

				tyran_string* string = TYRAN_CALLOC_TYPE(self->runtime->string_pool, tyran_string);
				tyran_string_init(string, self->runtime->memory, (tyran_string_char*) d, char_count);

				tyran_string_object_new(&destination, self->runtime, string);
			}
			break;
			case NIMBUS_EVENT_DEFINITION_OBJECT: {
				TYRAN_ERROR("To be implemented...");
			}
			break;
			case NIMBUS_EVENT_DEFINITION_SIZE2: {
				nimbus_property_writer_size2(writer, &destination, (nimbus_size2*)d);
			}
			break;
			case NIMBUS_EVENT_DEFINITION_SIZE2I: {
				nimbus_property_writer_size2i(writer, &destination, (nimbus_size2i*)d);
			}
			break;
			case NIMBUS_EVENT_DEFINITION_VECTOR3: {
				nimbus_property_writer_vector3(writer, &destination, (nimbus_vector3*)d);
			}
			break;
			case NIMBUS_EVENT_DEFINITION_VECTOR2: {
				nimbus_property_writer_vector2(writer, &destination, (nimbus_vector2*)d);
			}
			break;
			case NIMBUS_EVENT_DEFINITION_RECT2: {
				nimbus_property_writer_rect2(writer, &destination, (nimbus_rect*)d);
			}
			break;
			case NIMBUS_EVENT_DEFINITION_ROTATION: {
				nimbus_property_writer_quaternion(writer, &destination, (nimbus_quaternion*)d);
			}
			break;
		}
		tyran_object_insert(o, &p->symbol, &destination);
		tyran_value_release(destination);
	}

	return e->properties_count;
}
*/
