#include "event_to_object.h"

#include <tyranscript/tyran_symbol_table.h>
#include <tyran_core/event/event_stream.h>

#include <tyranscript/tyran_object.h>
#include "event_definition.h"
#include "object_info.h"

#include "property_writer.h"

#include <tyran_engine/type/size2.h>
#include <tyran_engine/type/size2i.h>
#include <tyran_engine/type/vector2.h>

#include <tyranscript/tyran_runtime.h>

#define NIMBUS_OBJECT_TO_EVENT_MEMBER_ALIGN() { const int alignment = 4; d += (alignment - ((intptr_t)(d-buf) % alignment)) % alignment; }


void nimbus_event_to_arguments_init(nimbus_event_to_arguments* self, struct tyran_symbol_table* symbol_table, tyran_runtime* runtime)
{
	tyran_symbol_table_add(symbol_table, &self->x_symbol, "x");
	tyran_symbol_table_add(symbol_table, &self->y_symbol, "y");
	tyran_symbol_table_add(symbol_table, &self->z_symbol, "z");
	tyran_symbol_table_add(symbol_table, &self->width_symbol, "width");
	tyran_symbol_table_add(symbol_table, &self->height_symbol, "height");

	self->runtime = runtime;

	nimbus_property_writer_init(&self->writer, runtime, symbol_table);
}

int nimbus_event_to_arguments_convert(nimbus_event_to_arguments* self, tyran_value* arguments, int max_arguments, nimbus_event_read_stream* stream, nimbus_event_definition* e)
{
	nimbus_property_writer* writer = &self->writer;

	const u8t* d = stream->pointer;
	const u8t* buf = d;
	if (e->has_index) {
		// ignored for now
		d += sizeof(int);
		NIMBUS_OBJECT_TO_EVENT_MEMBER_ALIGN();
	}

	for (int i = 0; i < e->properties_count; ++i) {
		nimbus_event_definition_property* p = &e->properties[i];
		tyran_value* destination = &arguments[i];
		switch (p->type) {
			case NIMBUS_EVENT_DEFINITION_FLOAT: {
				tyran_value_set_number(*destination, *((float*)d));
				d += sizeof(float);
			}
			break;
			case NIMBUS_EVENT_DEFINITION_INTEGER: {
				tyran_value_set_number(*destination, *((int*)d));
				d += sizeof(int);
			}
			break;
			case NIMBUS_EVENT_DEFINITION_STRING: {
				TYRAN_ERROR("To be implemented...");
			}
			break;
			case NIMBUS_EVENT_DEFINITION_OBJECT: {
				TYRAN_ERROR("To be implemented...");
			}
			break;
			case NIMBUS_EVENT_DEFINITION_SIZE2: {
				nimbus_property_writer_size2(writer, destination, (nimbus_size2*)d);
				d += sizeof(nimbus_vector2);
			}
			break;
			case NIMBUS_EVENT_DEFINITION_SIZE2I: {
				nimbus_property_writer_size2i(writer, destination, (nimbus_size2i*)d);
				d += sizeof(nimbus_vector2);
			}
			break;
			case NIMBUS_EVENT_DEFINITION_VECTOR3: {
				nimbus_property_writer_vector3(writer, destination, (nimbus_vector3*)d);
				d += sizeof(nimbus_vector3);
			}
			break;
			case NIMBUS_EVENT_DEFINITION_VECTOR2: {
				nimbus_property_writer_vector2(writer, destination, (nimbus_vector2*)d);
				d += sizeof(nimbus_vector2);
			}
			break;
			case NIMBUS_EVENT_DEFINITION_RECT2: {
				nimbus_property_writer_rect2(writer, destination, (nimbus_rect*)d);
				d += sizeof(nimbus_rect);
			}
			break;
			case NIMBUS_EVENT_DEFINITION_ROTATION: {
				nimbus_property_writer_quaternion(writer, destination, (nimbus_quaternion*)d);
				d += sizeof(nimbus_quaternion);
			}
			break;
		}
		NIMBUS_OBJECT_TO_EVENT_MEMBER_ALIGN();
	}

	return e->properties_count;
}

/*
static tyran_object* get_object_from_track_and_index(const script_world* w, tornado::object_index world_type, tornado::object_index track, tornado::object_index index)
{
	return w->types[world_type].tracks[track].objects[index];
}

tyran_value* convert_from_event_to_object(tyran_runtime* runtime, const script_world* w, const uint8* buf, int size, const tornado::class_definition& e, int use_index)
{
	const uint8* d = buf;

	tyran_value* o = tyran_value_new();

	tyran_value_set_object(*o, tyran_object_new(runtime));

	for (int i=0; i<e.property_count; ++i)
	{
		const tornado::class_property& p = e.properties[i];
		tyran_value* v = tyran_value_new();
		switch (p.type)
		{
			case tornado::class_property_type_float:
				tyran_value_set_number(*v, *((float*)d));
				d += sizeof(float);
			break;
			case tornado::class_property_type_integer:
				tyran_value_set_number(*v, *((int32*)d));
				d += sizeof(int32);
			break;
			case tornado::class_property_type_string:
			{
				TORNADO_ERROR("Not implemented");
			}
			break;
			case tornado::class_property_type_object:
			{
				tornado::object_index index;
				index = *((tornado::object_index*)d);
				d += sizeof(tornado::object_index);

				if (index == tornado::object_index_null)
				{
					v->type = TYRAN_VALUE_TYPE_UNDEFINED;
				}
				else
				{
					tyran_object* o = get_object_from_track_and_index(w, e.world_type, p.referring_to_object_compatible_with_track, index);
					tyran_value_set_object(*v, o);
				}
			}
			break;
			case tornado::class_property_type_array:
			{
				TORNADO_ERROR("TO BE IMPLEMENTED");
			}
			break;
			case tornado::class_property_type_vector2:
			{
				vector2* vec = ((vector2*)d);
				d += sizeof(vector2);

				tyran_value_set_object(*v, tyran_object_new(runtime));

				tyran_value* x = tyran_value_new();
				tyran_value_set_number(*x, vec->x);

				tyran_value* y = tyran_value_new();
				tyran_value_set_number(*y, vec->y);

				tyran_value_object_insert_key(v, (tyran_object_key*)tyran_string_from_c_str("x"), x);
				tyran_value_object_insert_key(v, (tyran_object_key*)tyran_string_from_c_str("y"), y);

				TORNADO_LOG(" vector2:" <<  vec->x << ", " << vec->y);
			}
			break;
			case tornado::class_property_type_vector:
			{
				vector3* vec = ((vector3*)d);
				d += sizeof(vector3);

				tyran_value_set_object(*v, tyran_object_new(runtime));

				tyran_value* x = tyran_value_new();
				tyran_value_set_number(*x, vec->x);

				tyran_value* y = tyran_value_new();
				tyran_value_set_number(*y, vec->y);

				tyran_value* z = tyran_value_new();
				tyran_value_set_number(*y, vec->z);

				tyran_value_object_insert_key(v, (tyran_object_key*)tyran_string_from_c_str("x"), x);
				tyran_value_object_insert_key(v, (tyran_object_key*)tyran_string_from_c_str("y"), y);
				tyran_value_object_insert_key(v, (tyran_object_key*)tyran_string_from_c_str("z"), z);

				TORNADO_LOG(" vector2:" <<  vec->x << ", " << vec->y << ", " << vec->z);
			}
			break;
			case tornado::class_property_type_size2:
			{
				size2* sz = ((size2*)d);
				d += sizeof(size2);

				tyran_value_set_object(*v, tyran_object_new(runtime));

				tyran_value* width = tyran_value_new();
				tyran_value_set_number(*width, sz->width);

				tyran_value* height = tyran_value_new();
				tyran_value_set_number(*height, sz->height);

				tyran_value_object_insert_key(v, (tyran_object_key*)tyran_string_from_c_str("width"), width);
				tyran_value_object_insert_key(v, (tyran_object_key*)tyran_string_from_c_str("height"), height);

				TORNADO_LOG(" size2i:" <<  sz->width << ", " << sz->height);
			}
			break;
			case tornado::class_property_type_size2i:
			{
				size2i* sz = ((size2i*)d);
				d += sizeof(size2i);

				tyran_value_set_object(*v, tyran_object_new(runtime));

				tyran_value* width = tyran_value_new();
				tyran_value_set_number(*width, sz->width);

				tyran_value* height = tyran_value_new();
				tyran_value_set_number(*height, sz->height);

				tyran_value_object_insert_key(v, (tyran_object_key*)tyran_string_from_c_str("width"), width);
				tyran_value_object_insert_key(v, (tyran_object_key*)tyran_string_from_c_str("height"), height);

				TORNADO_LOG(" size2i:" <<  sz->width << ", " << sz->height);
			}
			break;
			case tornado::class_property_type_rect2:
			{
				rect2* rt = ((rect2*)d);
				d += sizeof(rect2);

				tyran_value_set_object(*v, tyran_object_new(runtime));

				tyran_value* x = tyran_value_new();
				tyran_value_set_number(*x, rt->x);

				tyran_value* y = tyran_value_new();
				tyran_value_set_number(*y, rt->y);

				tyran_value* width = tyran_value_new();
				tyran_value_set_number(*width, rt->width);

				tyran_value* height = tyran_value_new();
				tyran_value_set_number(*height, rt->height);

				tyran_value_object_insert_key(v, (tyran_object_key*)tyran_string_from_c_str("x"), x);
				tyran_value_object_insert_key(v, (tyran_object_key*)tyran_string_from_c_str("y"), y);
				tyran_value_object_insert_key(v, (tyran_object_key*)tyran_string_from_c_str("width"), width);
				tyran_value_object_insert_key(v, (tyran_object_key*)tyran_string_from_c_str("height"), height);

				TORNADO_LOG(" vector2:" <<  rt->x << ", " << rt->y);
			}
			break;
			case tornado::class_property_type_rotation:
			{
				vector2* vec = ((vector2*)d);
				d += sizeof(vector2);

				tyran_value_set_object(*v, tyran_object_new(runtime));

				tyran_value* x = tyran_value_new();
				tyran_value_set_number(*x, vec->x);

				tyran_value* y = tyran_value_new();
				tyran_value_set_number(*y, vec->y);

				tyran_value_object_insert_key(v, (tyran_object_key*)tyran_string_from_c_str("x"), x);
				tyran_value_object_insert_key(v, (tyran_object_key*)tyran_string_from_c_str("y"), y);

				TORNADO_LOG(" vector2:" <<  vec->x << ", " << vec->y);
			}

			break;
			case tornado::class_property_type_callback_function:
			{
				TORNADO_ERROR("TO BE IMPLEMENTED");
			}
			break;
		}

		if (use_index)
		{
			tyran_value_object_insert_array(o, i, v);
		}
		else
		{
			tyran_value_object_insert_key(o, (tyran_object_key*)tyran_string_from_c_str(p.name), v);
		}
	}

	return o;
}
*/
int b;
