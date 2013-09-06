#include <tyran_engine/type/vector3.h>

nimbus_vector3 nimbus_vector3_make(tyran_number x, tyran_number y, tyran_number z)
{
	nimbus_vector3 point;

	point.x = x;
	point.y = y;
	point.z = z;

	return point;
}
