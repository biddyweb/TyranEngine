#include <tyran_engine/type/vector2.h>
#include <tyran_engine/math/nimbus_math.h>

nimbus_vector2 nimbus_vector2_make(tyran_number a, tyran_number b)
{
	nimbus_vector2 point;

	point.x = a;
	point.y = b;

	return point;
}

nimbus_vector2 nimbus_vector2_mul_scalar(nimbus_vector2 p, tyran_number scalar)
{
	return nimbus_vector2_make(p.x * scalar, p.y * scalar);
}

nimbus_vector2 nimbus_vector2_add(nimbus_vector2 a, nimbus_vector2 b)
{
	return nimbus_vector2_make(a.x + b.x, a.y + b.y);
}

tyran_number nimbus_vector2_length(nimbus_vector2 a)
{
	return nimbus_math_sqrt(a.x * a.x + a.y * a.y);
}
