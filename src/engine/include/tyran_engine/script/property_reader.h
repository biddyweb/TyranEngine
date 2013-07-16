#ifndef nimbus_property_reader_h
#define nimbus_property_reader_h

#include <tyranscript/tyran_symbol.h>

#include <tyran_engine/type/vector3.h>
#include <tyran_engine/type/rect2.h>
#include <tyran_engine/type/vector2.h>
#include <tyran_engine/type/size2.h>
#include <tyran_engine/type/size2i.h>
#include <tyran_engine/type/quaternion.h>

struct tyran_symbol_table;
struct tyran_object;

typedef struct nimbus_property_reader {
	tyran_symbol x_symbol;
	tyran_symbol y_symbol;
	tyran_symbol z_symbol;
	tyran_symbol width_symbol;
	tyran_symbol height_symbol;
} nimbus_property_reader;

void nimbus_property_reader_init(nimbus_property_reader* self, struct tyran_symbol_table* symbol_table);
void nimbus_property_reader_float(nimbus_property_reader* self, float* v, struct tyran_object* o, tyran_symbol* s);

void nimbus_property_reader_vector3(nimbus_property_reader* self, nimbus_vector3* v, struct tyran_object* o, tyran_symbol* s);
void nimbus_property_reader_vector2(nimbus_property_reader* self, nimbus_vector2* v, struct tyran_object* o, tyran_symbol* s);
void nimbus_property_reader_rect2(nimbus_property_reader* self, nimbus_rect2* v, struct tyran_object* o, tyran_symbol* s);
void nimbus_property_reader_size2(nimbus_property_reader* self, nimbus_size2* v, struct tyran_object* o, tyran_symbol* s);
void nimbus_property_reader_size2i(nimbus_property_reader* self, nimbus_size2i* v, struct tyran_object* o, tyran_symbol* s);
void nimbus_property_reader_quaternion(nimbus_property_reader* self, nimbus_quaternion* v, struct tyran_object* o, tyran_symbol* s);

#endif
