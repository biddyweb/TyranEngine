#include <tyran_engine/script/property_reader.h>
#include <tyranscript/tyran_symbol_table.h>
#include <tyranscript/tyran_object.h>

#define FETCH_OBJECT(symbol) tyran_object* value_object; { tyran_value __value; tyran_object_lookup_prototype(&__value, o, symbol); value_object = tyran_value_object(&__value); }

#define FETCH_OBJECT_NUMBER(destination, source_symbol) { tyran_value __value; tyran_object_lookup_prototype(&__value, value_object, &source_symbol); destination = tyran_value_number(&__value); }

void nimbus_property_reader_init(nimbus_property_reader* self,  tyran_symbol_table* symbol_table)
{
	tyran_symbol_table_add(symbol_table, &self->x_symbol, "x");
	tyran_symbol_table_add(symbol_table, &self->y_symbol, "y");
	tyran_symbol_table_add(symbol_table, &self->z_symbol, "z");
	tyran_symbol_table_add(symbol_table, &self->width_symbol, "width");
	tyran_symbol_table_add(symbol_table, &self->height_symbol, "height");
}

void nimbus_property_reader_float(nimbus_property_reader* self, float* v, tyran_object* o, tyran_symbol* s)
{
	tyran_value value;
	tyran_object_lookup_prototype(&value, o, s);
	*v = tyran_value_number(&value);
}

void nimbus_property_reader_vector3(nimbus_property_reader* self, nimbus_vector3* v, tyran_object* o, tyran_symbol* s)
{
	FETCH_OBJECT(s);

	TYRAN_LOG("Vector3 object:%p prototype:%p", value_object, tyran_object_get_prototype(value_object));


	FETCH_OBJECT_NUMBER(v->x, self->x_symbol);
	FETCH_OBJECT_NUMBER(v->y, self->y_symbol);
	FETCH_OBJECT_NUMBER(v->z, self->z_symbol);

	TYRAN_LOG("Read vector3: %f, %f, %f", v->x, v->y, v->z);
}

void nimbus_property_reader_vector2(nimbus_property_reader* self, nimbus_vector2* v, tyran_object* o, tyran_symbol* s)
{
	FETCH_OBJECT(s);
	FETCH_OBJECT_NUMBER(v->x, self->x_symbol);
	FETCH_OBJECT_NUMBER(v->y, self->y_symbol);
}

void nimbus_property_reader_rect2(nimbus_property_reader* self, nimbus_rect* v, tyran_object* o, tyran_symbol* s)
{
	FETCH_OBJECT(s);
	FETCH_OBJECT_NUMBER(v->vector.x, self->x_symbol);
	FETCH_OBJECT_NUMBER(v->vector.y, self->y_symbol);
	FETCH_OBJECT_NUMBER(v->size.width, self->width_symbol);
	FETCH_OBJECT_NUMBER(v->size.height, self->height_symbol);
}

void nimbus_property_reader_size2(nimbus_property_reader* self, nimbus_size2* v, tyran_object* o, tyran_symbol* s)
{
	FETCH_OBJECT(s);
	FETCH_OBJECT_NUMBER(v->width, self->width_symbol);
	FETCH_OBJECT_NUMBER(v->height, self->height_symbol);
}

void nimbus_property_reader_size2i(nimbus_property_reader* self, nimbus_size2i* v, tyran_object* o, tyran_symbol* s)
{
	FETCH_OBJECT(s);
	FETCH_OBJECT_NUMBER(v->width, self->width_symbol);
	FETCH_OBJECT_NUMBER(v->height, self->height_symbol);
}

void nimbus_property_reader_quaternion(nimbus_property_reader* self, nimbus_quaternion* q, tyran_object* o, tyran_symbol* s)
{
	nimbus_vector3 v;

	FETCH_OBJECT(s);
	FETCH_OBJECT_NUMBER(v.x, self->x_symbol);
	FETCH_OBJECT_NUMBER(v.y, self->y_symbol);
	FETCH_OBJECT_NUMBER(v.z, self->z_symbol);

	nimbus_quaternion_from_euler(q, v.x, v.y, v.z);
}
