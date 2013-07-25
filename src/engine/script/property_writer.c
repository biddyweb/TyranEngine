#include "property_writer.h"
#include <tyran_engine/script/property_reader.h>
#include <tyranscript/tyran_symbol_table.h>
#include <tyranscript/tyran_object.h>
#include <tyranscript/tyran_value.h>

#define CREATE_OBJECT() tyran_object* value_object; value_object = tyran_object_new(self->runtime); tyran_value_set_object(*destination, value_object);

#define SET_OBJECT_NUMBER(destination_symbol, source_number) { tyran_value __value; tyran_value_set_number(__value, source_number); tyran_object_insert(value_object, &destination_symbol, &__value); }

void nimbus_property_writer_init(nimbus_property_writer* self, struct tyran_runtime* runtime, tyran_symbol_table* symbol_table)
{
	self->runtime = runtime;
	tyran_symbol_table_add(symbol_table, &self->x_symbol, "x");
	tyran_symbol_table_add(symbol_table, &self->y_symbol, "y");
	tyran_symbol_table_add(symbol_table, &self->z_symbol, "z");
	tyran_symbol_table_add(symbol_table, &self->width_symbol, "width");
	tyran_symbol_table_add(symbol_table, &self->height_symbol, "height");
}

void nimbus_property_writer_float(nimbus_property_writer* self, float* v, tyran_object* o, tyran_symbol* s)
{
	tyran_value value;
	tyran_object_lookup_prototype(&value, o, s);
	*v = tyran_value_number(&value);
}

void nimbus_property_writer_vector3(nimbus_property_writer* self, tyran_value* destination, nimbus_vector3* v)
{
	CREATE_OBJECT();
	SET_OBJECT_NUMBER(self->x_symbol, v->x);
	SET_OBJECT_NUMBER(self->y_symbol, v->y);
	SET_OBJECT_NUMBER(self->z_symbol, v->z);
}

void nimbus_property_writer_vector2(nimbus_property_writer* self, tyran_value* destination, nimbus_vector2* v)
{
	CREATE_OBJECT();
	SET_OBJECT_NUMBER(self->x_symbol, v->x);
	SET_OBJECT_NUMBER(self->y_symbol, v->y);
}

void nimbus_property_writer_rect2(nimbus_property_writer* self, tyran_value* destination, nimbus_rect* v)
{
	CREATE_OBJECT();
	SET_OBJECT_NUMBER(self->x_symbol, v->vector.x);
	SET_OBJECT_NUMBER(self->y_symbol, v->vector.y);
	SET_OBJECT_NUMBER(self->width_symbol, v->size.width);
	SET_OBJECT_NUMBER(self->height_symbol, v->size.height);
}

void nimbus_property_writer_size2(nimbus_property_writer* self, tyran_value* destination, nimbus_size2* v)
{
	CREATE_OBJECT();
	SET_OBJECT_NUMBER(self->width_symbol, v->width);
	SET_OBJECT_NUMBER(self->height_symbol, v->height);
}

void nimbus_property_writer_size2i(nimbus_property_writer* self, tyran_value* destination, nimbus_size2i* v)
{
	CREATE_OBJECT();
	SET_OBJECT_NUMBER(self->width_symbol, v->width);
	SET_OBJECT_NUMBER(self->height_symbol, v->height);
}

void nimbus_property_writer_quaternion(nimbus_property_writer* self, tyran_value* destination, nimbus_quaternion* q)
{
	nimbus_vector3 v;

	nimbus_quaternion_to_euler(q, &v.x, &v.y, &v.z);

	CREATE_OBJECT();
	SET_OBJECT_NUMBER(self->x_symbol, v.x);
	SET_OBJECT_NUMBER(self->y_symbol, v.y);
	SET_OBJECT_NUMBER(self->z_symbol, v.z);
}
