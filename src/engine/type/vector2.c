#include <tyran_engine/type/vector2.h>
#include <tyran_engine/math/nimbus_math.h>
#include <tyranscript/tyran_clib.h>

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

void nimbus_vector2_scaleadd(nimbus_vector2* result, nimbus_vector2* a, nimbus_vector2* b, float scale)
{
	result->x = a->x + b->x * scale;
	result->y = a->y + b->y * scale;
}

void nimbus_vector2_zero(nimbus_vector2* result)
{
	tyran_mem_clear(result, 0);
}

nimbus_vector2 nimbus_vector2_sub(nimbus_vector2 a, nimbus_vector2 b)
{
	return nimbus_vector2_make(a.x - b.x, a.y - b.y);
}

void nimbus_vector2_subtract(nimbus_vector2* result, nimbus_vector2* a, nimbus_vector2* b)
{
	result->x = a->x - b->x;
	result->y = a->y - b->y;
}

tyran_number nimbus_vector2_length(nimbus_vector2 a)
{
	return nimbus_math_sqrt(a.x * a.x + a.y * a.y);
}

nimbus_vector2 nimbus_vector2_unit(nimbus_vector2 a)
{
	tyran_number length = nimbus_vector2_length(a);
	nimbus_vector2 result;
	result.x = a.x / length;
	result.y = a.y / length;

	return result;
}

tyran_number nimbus_vector2_dot(nimbus_vector2 a, nimbus_vector2 b)
{
	return a.x * b.x + a.y * b.y;
}

nimbus_vector2 nimbus_vector2_normal(nimbus_vector2 a, nimbus_vector2 b)
{
	return nimbus_vector2_unit(nimbus_vector2_make(a.y - b.y, b.x - a.x));
}
