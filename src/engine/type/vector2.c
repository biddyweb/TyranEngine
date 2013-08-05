#include <tyran_engine/type/vector2.h>

nimbus_vector2 nimbus_vector2_make(tyran_number a, tyran_number b)
{
	nimbus_vector2 point;

	point.x = a;
	point.y = b;

	return point;
}
