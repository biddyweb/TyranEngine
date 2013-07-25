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

void nimbus_quaternion_from_euler(nimbus_quaternion* q, tyran_number roll, tyran_number pitch, tyran_number yaw)
{
	tyran_number p = pitch / 2.0f;
	tyran_number y = yaw / 2.0f;
	tyran_number r = roll / 2.0f;

	tyran_number sinp = nimbus_math_sin(p);
	tyran_number siny = nimbus_math_sin(y);
	tyran_number sinr = nimbus_math_sin(r);
	tyran_number cosp = nimbus_math_cos(p);
	tyran_number cosy = nimbus_math_cos(y);
	tyran_number cosr = nimbus_math_cos(r);

	q->v.x = sinr * cosp * cosy - cosr * sinp * siny;
	q->v.y = cosr * sinp * cosy + sinr * cosp * siny;
	q->v.z = cosr * cosp * siny - sinr * sinp * cosy;
	q->w = cosr * cosp * cosy + sinr * sinp * siny;

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
