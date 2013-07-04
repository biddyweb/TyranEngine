/*
static tyran_object* get_object_from_track_and_index(const script_world* w, tornado::object_index world_type, tornado::object_index track, tornado::object_index index)
{
	return w->types[world_type].tracks[track].objects[index];
}

tyran_value* convert_from_event_to_object(tyran_runtime* runtime, const script_world* w, const uint8* buf, int size, const tornado::class_definition& e, int use_index)
{
	const uint8* d = buf;

	tyran_value* o = tyran_value_new();

	tyran_value_set_object(*o, tyran_object_new(runtime));

	for (int i=0; i<e.property_count; ++i)
	{
		const tornado::class_property& p = e.properties[i];
		tyran_value* v = tyran_value_new();
		switch (p.type)
		{
			case tornado::class_property_type_float:
				tyran_value_set_number(*v, *((float*)d));
				d += sizeof(float);
			break;
			case tornado::class_property_type_integer:
				tyran_value_set_number(*v, *((int32*)d));
				d += sizeof(int32);
			break;
			case tornado::class_property_type_string:
			{
				TORNADO_ERROR("Not implemented");
			}
			break;
			case tornado::class_property_type_object:
			{
				tornado::object_index index;
				index = *((tornado::object_index*)d);
				d += sizeof(tornado::object_index);

				if (index == tornado::object_index_null)
				{
					v->type = TYRAN_VALUE_TYPE_UNDEFINED;
				}
				else
				{
					tyran_object* o = get_object_from_track_and_index(w, e.world_type, p.referring_to_object_compatible_with_track, index);
					tyran_value_set_object(*v, o);
				}
			}
			break;
			case tornado::class_property_type_array:
			{
				TORNADO_ERROR("TO BE IMPLEMENTED");
			}
			break;
			case tornado::class_property_type_vector2:
			{
				vector2* vec = ((vector2*)d);
				d += sizeof(vector2);

				tyran_value_set_object(*v, tyran_object_new(runtime));

				tyran_value* x = tyran_value_new();
				tyran_value_set_number(*x, vec->x);

				tyran_value* y = tyran_value_new();
				tyran_value_set_number(*y, vec->y);

				tyran_value_object_insert_key(v, (tyran_object_key*)tyran_string_from_c_str("x"), x);
				tyran_value_object_insert_key(v, (tyran_object_key*)tyran_string_from_c_str("y"), y);

				TORNADO_LOG(" vector2:" <<  vec->x << ", " << vec->y);
			}
			break;
			case tornado::class_property_type_vector:
			{
				vector3* vec = ((vector3*)d);
				d += sizeof(vector3);

				tyran_value_set_object(*v, tyran_object_new(runtime));

				tyran_value* x = tyran_value_new();
				tyran_value_set_number(*x, vec->x);

				tyran_value* y = tyran_value_new();
				tyran_value_set_number(*y, vec->y);

				tyran_value* z = tyran_value_new();
				tyran_value_set_number(*y, vec->z);

				tyran_value_object_insert_key(v, (tyran_object_key*)tyran_string_from_c_str("x"), x);
				tyran_value_object_insert_key(v, (tyran_object_key*)tyran_string_from_c_str("y"), y);
				tyran_value_object_insert_key(v, (tyran_object_key*)tyran_string_from_c_str("z"), z);

				TORNADO_LOG(" vector2:" <<  vec->x << ", " << vec->y << ", " << vec->z);
			}
			break;
			case tornado::class_property_type_size2:
			{
				size2* sz = ((size2*)d);
				d += sizeof(size2);

				tyran_value_set_object(*v, tyran_object_new(runtime));

				tyran_value* width = tyran_value_new();
				tyran_value_set_number(*width, sz->width);

				tyran_value* height = tyran_value_new();
				tyran_value_set_number(*height, sz->height);

				tyran_value_object_insert_key(v, (tyran_object_key*)tyran_string_from_c_str("width"), width);
				tyran_value_object_insert_key(v, (tyran_object_key*)tyran_string_from_c_str("height"), height);

				TORNADO_LOG(" size2i:" <<  sz->width << ", " << sz->height);
			}
			break;
			case tornado::class_property_type_size2i:
			{
				size2i* sz = ((size2i*)d);
				d += sizeof(size2i);

				tyran_value_set_object(*v, tyran_object_new(runtime));

				tyran_value* width = tyran_value_new();
				tyran_value_set_number(*width, sz->width);

				tyran_value* height = tyran_value_new();
				tyran_value_set_number(*height, sz->height);

				tyran_value_object_insert_key(v, (tyran_object_key*)tyran_string_from_c_str("width"), width);
				tyran_value_object_insert_key(v, (tyran_object_key*)tyran_string_from_c_str("height"), height);

				TORNADO_LOG(" size2i:" <<  sz->width << ", " << sz->height);
			}
			break;
			case tornado::class_property_type_rect2:
			{
				rect2* rt = ((rect2*)d);
				d += sizeof(rect2);

				tyran_value_set_object(*v, tyran_object_new(runtime));

				tyran_value* x = tyran_value_new();
				tyran_value_set_number(*x, rt->x);

				tyran_value* y = tyran_value_new();
				tyran_value_set_number(*y, rt->y);

				tyran_value* width = tyran_value_new();
				tyran_value_set_number(*width, rt->width);

				tyran_value* height = tyran_value_new();
				tyran_value_set_number(*height, rt->height);

				tyran_value_object_insert_key(v, (tyran_object_key*)tyran_string_from_c_str("x"), x);
				tyran_value_object_insert_key(v, (tyran_object_key*)tyran_string_from_c_str("y"), y);
				tyran_value_object_insert_key(v, (tyran_object_key*)tyran_string_from_c_str("width"), width);
				tyran_value_object_insert_key(v, (tyran_object_key*)tyran_string_from_c_str("height"), height);

				TORNADO_LOG(" vector2:" <<  rt->x << ", " << rt->y);
			}
			break;
			case tornado::class_property_type_rotation:
			{
				vector2* vec = ((vector2*)d);
				d += sizeof(vector2);

				tyran_value_set_object(*v, tyran_object_new(runtime));

				tyran_value* x = tyran_value_new();
				tyran_value_set_number(*x, vec->x);

				tyran_value* y = tyran_value_new();
				tyran_value_set_number(*y, vec->y);

				tyran_value_object_insert_key(v, (tyran_object_key*)tyran_string_from_c_str("x"), x);
				tyran_value_object_insert_key(v, (tyran_object_key*)tyran_string_from_c_str("y"), y);

				TORNADO_LOG(" vector2:" <<  vec->x << ", " << vec->y);
			}

			break;
			case tornado::class_property_type_callback_function:
			{
				TORNADO_ERROR("TO BE IMPLEMENTED");
			}
			break;
		}

		if (use_index)
		{
			tyran_value_object_insert_array(o, i, v);
		}
		else
		{
			tyran_value_object_insert_key(o, (tyran_object_key*)tyran_string_from_c_str(p.name), v);
		}
	}

	return o;
}
*/
int b;
