#ifndef nimbus_property_writer_h
#define nimbus_property_writer_h

#include <tyranscript/tyran_symbol.h>

#include <tyran_engine/type/vector3.h>
#include <tyran_engine/type/rect2.h>
#include <tyran_engine/type/vector2.h>
#include <tyran_engine/type/size2.h>
#include <tyran_engine/type/size2i.h>
#include <tyran_engine/type/quaternion.h>

struct tyran_symbol_table;
struct tyran_object;
struct tyran_value;
struct tyran_runtime;

typedef struct nimbus_property_writer {
	tyran_symbol x_symbol;
	tyran_symbol y_symbol;
	tyran_symbol z_symbol;
	tyran_symbol width_symbol;
	tyran_symbol height_symbol;
	struct tyran_runtime* runtime;
} nimbus_property_writer;

void nimbus_property_writer_init(nimbus_property_writer* self, struct tyran_runtime* runtime, struct tyran_symbol_table* symbol_table);
void nimbus_property_writer_float(nimbus_property_writer* self, float* v, struct tyran_object* o, tyran_symbol* s);
void nimbus_property_writer_vector3(nimbus_property_writer* self, struct tyran_value* destination, nimbus_vector3* v);
void nimbus_property_writer_vector2(nimbus_property_writer* self, struct tyran_value* destination, nimbus_vector2* v);
void nimbus_property_writer_rect2(nimbus_property_writer* self, struct tyran_value* destination, nimbus_rect* v);
void nimbus_property_writer_size2(nimbus_property_writer* self, struct tyran_value* destination, nimbus_size2* v);
void nimbus_property_writer_size2i(nimbus_property_writer* self, struct tyran_value* destination, nimbus_size2i* v);
void nimbus_property_writer_quaternion(nimbus_property_writer* self, struct tyran_value* destination, nimbus_quaternion* q);

#endif
