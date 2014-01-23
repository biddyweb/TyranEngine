#include <tyran_engine/type/quaternion.h>
#include <tyran_engine/math/nimbus_math.h>

void nimbus_quaternion_normalize(nimbus_quaternion* q)
{
	tyran_number scale = nimbus_math_sqrt(q->v.x * q->v.x + q->v.y * q->v.y + q->v.z * q->v.z + q->w * q->w);
	if (nimbus_math_fabs(scale) < 0.0001f) {
		return;
	}
	q->v.x /= scale;
	q->v.y /= scale;
	q->v.z /= scale;
	q->w /= scale;
}

void nimbus_quaternion_from_euler(nimbus_quaternion* q, tyran_number z, tyran_number x, tyran_number y)
{
	tyran_number half_x = x / 2.0f;
	tyran_number half_y = y / 2.0f;
	tyran_number half_z = z / 2.0f;

	tyran_number sin_x = nimbus_math_sin(half_x);
	tyran_number sin_y = nimbus_math_sin(half_y);
	tyran_number sin_z = nimbus_math_sin(half_z);

	tyran_number cos_x = nimbus_math_cos(half_x);
	tyran_number cos_y = nimbus_math_cos(half_y);
	tyran_number cos_z = nimbus_math_cos(half_z);

	q->w = cos_x * cos_y * cos_z - sin_x * sin_y * sin_z;
	q->v.x = sin_x * sin_y * cos_z + cos_x * cos_y * sin_z;
	q->v.y = sin_x * cos_y * cos_z + cos_x * sin_y * sin_z;
	q->v.z = cos_x * sin_y * cos_z - sin_x * cos_y * sin_z;

	nimbus_quaternion_normalize(q);
}

void nimbus_quaternion_to_euler(nimbus_quaternion* q, tyran_number* roll, tyran_number* pitch, tyran_number* yaw)
{
	float sqz = q->v.z * q->v.z;
	float sqw = q->w * q->w;
	float sqy = q->v.y * q->v.y;

	*yaw = nimbus_math_atan2(2.0f * q->v.x * q->w + 2.0f * q->v.y * q->v.z, 1.0f - 2.0f * (sqz  + sqw));
	*pitch = nimbus_math_asin(2.0f * ( q->v.x * q->v.z - q->w * q->v.y ) );
	*roll = nimbus_math_atan2(2.0f * q->v.x * q->v.y + 2.0f * q->v.z * q->w, 1.0f - 2.0f * (sqy + sqz));
}
