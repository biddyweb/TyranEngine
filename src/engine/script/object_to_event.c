#include "object_to_event.h"

#include <tyranscript/tyran_symbol_table.h>
#include <tyran_core/event/event_stream.h>

#include <tyranscript/tyran_object.h>
#include <tyran_engine/event_definition/event_definition.h>
#include "object_info.h"
#include <tyranscript/tyran_string.h>

#include <tyran_engine/script/property_reader.h>

#include <tyran_engine/type/size2.h>
#include <tyran_engine/type/size2i.h>
#include <tyran_engine/type/vector2.h>

#include <stdint.h>

#define NIMBUS_OBJECT_TO_EVENT_MEMBER_ALIGN() { const int alignment = 4; d += (alignment - ((intptr_t)(d) % alignment)) % alignment; }

void nimbus_object_to_event_init(nimbus_object_to_event* self, struct tyran_memory* memory, tyran_symbol_table* symbol_table)
{
	nimbus_property_reader_init(&self->property_reader, symbol_table);
	self->temp_buf_max_size = 1024;
	self->temp_buf = TYRAN_MEMORY_ALLOC(memory, self->temp_buf_max_size, "event creation buffer");
	self->symbol_table = symbol_table;
}

void nimbus_object_to_event_free(nimbus_object_to_event* self)
{
	tyran_free(self->temp_buf);
}

void nimbus_object_to_event_convert(nimbus_object_to_event* self, nimbus_event_write_stream* stream, const struct tyran_object* o, const nimbus_event_definition* e)
{
	nimbus_object_info* info = (nimbus_object_info*) tyran_object_program_specific(o);
	nimbus_property_reader* reader = &self->property_reader;
	const u8t* buf = self->temp_buf;

	u8t* d = self->temp_buf;

	if (e->has_index) {
		int index = info->instance_index; // world->types[info->world_index].tracks[info->track_index].get_object_index(_this);
		TYRAN_ASSERT(index != -1,  "Index can not be null. tyran_object has not been assigned an index");
		*(int*) d = index;
		d += sizeof(int);
		NIMBUS_OBJECT_TO_EVENT_MEMBER_ALIGN();
	}

	const tyran_value* value;
	for (int i = 0; i < e->properties_count; ++i) {
		const nimbus_event_definition_property* p = &e->properties[i];
		// const char* debug_string = tyran_symbol_table_lookup(self->symbol_table, &p->symbol);
		// TYRAN_LOG("convert: '%s'", debug_string);
		switch (p->type) {
			case NIMBUS_EVENT_DEFINITION_FLOAT: {
				tyran_object_lookup_prototype(&value, o, &p->symbol);
				*((float*)d) = tyran_value_number(value);
				d += sizeof(float);
			}
			break;
			case NIMBUS_EVENT_DEFINITION_BOOLEAN: {
				tyran_object_lookup_prototype(&value, o, &p->symbol);
				*((tyran_boolean*)d) = tyran_value_boolean(value);
				d += sizeof(tyran_boolean);
			}
			break;
			case NIMBUS_EVENT_DEFINITION_INTEGER: {
				tyran_object_lookup_prototype(&value, o, &p->symbol);
				*((int*)d) = (int) tyran_value_number(value);
				d += sizeof(int);
			}
			break;
			case NIMBUS_EVENT_DEFINITION_SYMBOL: {
				tyran_object_lookup_prototype(&value, o, &p->symbol);
				*((tyran_symbol*)d) = tyran_value_symbol(value);
				d += sizeof(tyran_symbol);
			}
			break;
			case NIMBUS_EVENT_DEFINITION_STRING: {
				tyran_object_lookup_prototype(&value, o, &p->symbol);
				const tyran_string* str = tyran_value_string(value);
				*((tyran_string_length_type*)d) = str->len;
				d += sizeof(tyran_string_length_type);
				int octet_length = sizeof(tyran_string_char) * str->len;
				tyran_memcpy_octets(d, str->buf, octet_length);
				d += octet_length;
			}
			break;
			case NIMBUS_EVENT_DEFINITION_OBJECT: {
				int index = -1;
				tyran_object_lookup_prototype(&value, o, &p->symbol);
				if (!tyran_value_is_nil(value)) {
					const tyran_object* value_object = tyran_value_object(value);
					const nimbus_object_info* referenced_info = tyran_object_program_specific(value_object);
					index = referenced_info->instance_index;
				}
				*(int*)d = index;
				d += sizeof(int);
			}
			break;
			case NIMBUS_EVENT_DEFINITION_SIZE2: {
				nimbus_property_reader_size2(reader, (nimbus_size2*)d, o, &p->symbol);
				d += sizeof(nimbus_vector2);
			}
			break;
			case NIMBUS_EVENT_DEFINITION_SIZE2I: {
				nimbus_property_reader_size2i(reader, (nimbus_size2i*)d, o, &p->symbol);
				d += sizeof(nimbus_vector2);
			}
			break;
			case NIMBUS_EVENT_DEFINITION_VECTOR3: {
				nimbus_property_reader_vector3(reader, (nimbus_vector3*)d, o, &p->symbol);
				d += sizeof(nimbus_vector3);
			}
			break;
			case NIMBUS_EVENT_DEFINITION_VECTOR2: {
				nimbus_property_reader_vector2(reader, (nimbus_vector2*)d, o, &p->symbol);
				d += sizeof(nimbus_vector2);
			}
			break;
			case NIMBUS_EVENT_DEFINITION_RECT2: {
				nimbus_property_reader_rect2(reader, (nimbus_rect*)d, o, &p->symbol);
				d += sizeof(nimbus_rect);
			}
			break;
			case NIMBUS_EVENT_DEFINITION_ROTATION: {
				nimbus_property_reader_quaternion(reader, (nimbus_quaternion*)d, o, &p->symbol);
				d += sizeof(nimbus_quaternion);
			}
			break;
		}
		NIMBUS_OBJECT_TO_EVENT_MEMBER_ALIGN();
	}

	int octets_written = d - buf;

	nimbus_event_stream_write_event_header(stream, e->event_type_id);
	nimbus_event_stream_write_align(stream);
	nimbus_event_stream_write_octets(stream, buf, octets_written);
	nimbus_event_stream_write_event_end(stream);
}
