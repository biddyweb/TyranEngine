/*
int convert_from_object_to_event(uint8* buf, int max_size, tyran_object* _this, tyran_object* o, const tornado::class_definition& e, const script_world* world)
{
	uint8* d = (uint8*) buf;
	script_object_info* info = (script_object_info*)_this->program_specific;

	tyran_value tempValue;
	tempValue.type = TYRAN_VALUE_TYPE_OBJECT;
	tempValue.data.object = o;

	// print_value("Convert from object to event", &tempValue, 1);

	if (e.is_library_class)
	{
		TORNADO_LOG("Library class!");
		*((tornado::object_class_id_hash_type*)d) = e.class_id.do_not_get_this_value();
		TORNADO_LOG("Writing class hash:" << *(tornado::object_class_id_hash_type*)d << " debug:" << e.class_id.debug_class_name);
		d += sizeof(tornado::object_class_id_hash_type);
		d += sizeof(tornado::object_class_id_hash_type) % 4;

		return (d-buf);
	}

	if (e.has_index)
	{
		tornado::object_index index = world->types[info->script_class->world_type].tracks[info->script_class->world_track].get_object_index(_this);
		TORNADO_ASSERT(index != tornado::object_index_null,  "Index can not be null. tyran_object has not been assigned an index");
		//TORNADO_LOG(" writing index:" << index);
		*((tornado::object_index*)d) = index;
		d += sizeof(tornado::object_index);
	}

	char name[256];
	char value[256];

	for (int i = 0; i < e.property_count; ++i)
	{
		const tornado::class_property& p = e.properties[i];
		tyran_object_key_flag_type dummy_flag;
		name[0] = 0;
		value[0] = 0;
		tornado_sprintf(name, "%s", p.name);
		const tyran_value* v = tyran_object_lookup_prototype(o, (const tyran_object_key*)tyran_string_from_c_str(p.name), &dummy_flag);
		TORNADO_ASSERT(v != 0, "Couldn't find property '" << p.name << "'");
		switch (p.type)
		{
			case tornado::class_property_type_float:
				tornado_sprintf(value, "float:%f", v->data.number);
				*((float*)d) = (float) v->data.number;
				d += sizeof(float);
			break;
			case tornado::class_property_type_integer:
				tornado_sprintf(value, "int:%d", (int32)v->data.number);
				*((int32*)d) = (int32) v->data.number;
				d += sizeof(int32);
			break;
			case tornado::class_property_type_string:
			{
				char c_buf[128];
				tyran_string_to_c_str(c_buf, 128, v->data.str);
				tornado_sprintf(value, " string:'%s'", c_buf);
				int octet_length = TYRAN_UNICODE_STRLEN(v->data.str) * sizeof(tyran_string) + sizeof(tyran_string_length_type);
				tornado_memcpy(d, ((char*)v->data.str - sizeof(tyran_string_length_type)), octet_length);
				d += octet_length + (octet_length % 4);
			}
			break;
			case tornado::class_property_type_object:
			{
				tornado::object_index index;
				if (v->type == TYRAN_VALUE_TYPE_NULL)
				{
					index = tornado::object_index_null;
				}
				else
				{
					TORNADO_ASSERT(v->type == TYRAN_VALUE_TYPE_OBJECT, "Must be object");
					script_object_info* referenced_info = (script_object_info*)v->data.object->program_specific;
					TYRAN_ASSERT(referenced_info != 0, "Referencing an object that hasn't been introduced!");
					TORNADO_ASSERT(referenced_info->script_class->world_type == e.world_type, "Can't reference another engine world");
					index = world->types[referenced_info->script_class->world_type].tracks[referenced_info->script_class->world_track].get_object_index(v->data.object);
				}
				tornado_sprintf(value, " object reference:%d", index);
				*((tornado::object_index*)d) = index;
				d += sizeof(tornado::object_index);
			}
			break;
			case tornado::class_property_type_array:
			{
				TORNADO_ERROR("TO BE IMPLEMENTED");
			}
			break;
			case tornado::class_property_type_vector2:
			{
				tyran_value* x = tyran_value_object_lookup(v, (const tyran_object_key*)tyran_string_from_c_str("x"), &dummy_flag);
				((vector2*)d)->x = (number) x->data.number;

				tyran_value* y = tyran_value_object_lookup(v, (const tyran_object_key*)tyran_string_from_c_str("y"), &dummy_flag);
				((vector2*)d)->y = (number) y->data.number;

				tornado_sprintf(value, " vector2:%f, %f", ((vector2*)d)->x, ((vector2*)d)->y);

				d += sizeof(vector2);
			}
			break;
			case tornado::class_property_type_vector:
			{
				tyran_value* x = tyran_value_object_lookup(v, (tyran_object_key*)tyran_string_from_c_str("x"), &dummy_flag);
				((vector3*)d)->x = (number) x->data.number;

				tyran_value* y = tyran_value_object_lookup(v, (tyran_object_key*)tyran_string_from_c_str("y"), &dummy_flag);
				((vector3*)d)->y = (number) y->data.number;

				tyran_value* z = tyran_value_object_lookup(v, (tyran_object_key*)tyran_string_from_c_str("z"), &dummy_flag);
				((vector3*)d)->z = (number) z->data.number;

				tornado_sprintf(value, " vector3:%f, %f, %f", ((vector3*)d)->x, ((vector3*)d)->y, ((vector3*)d)->z );
				d += sizeof(vector3);
			}
			break;
			case tornado::class_property_type_size2:
			{
				tyran_value* x = tyran_value_object_lookup(v, (tyran_object_key*)tyran_string_from_c_str("width"), &dummy_flag);
				((size2*)d)->width = (number) x->data.number;

				tyran_value* y = tyran_value_object_lookup(v, (tyran_object_key*)tyran_string_from_c_str("height"), &dummy_flag);
				((size2*)d)->height = (number) y->data.number;

				tornado_sprintf(value, " size2:%f, %f", ((size2*)d)->width, ((size2*)d)->height);

				d += sizeof(size2);
			}
			break;
			case tornado::class_property_type_size2i:
			{
				tyran_value* x = tyran_value_object_lookup(v, (tyran_object_key*)tyran_string_from_c_str("width"), &dummy_flag);
				((size2i*)d)->width = (uint16) x->data.number;

				tyran_value* y = tyran_value_object_lookup(v, (tyran_object_key*)tyran_string_from_c_str("height"), &dummy_flag);
				((size2i*)d)->height = (uint16) y->data.number;

				tornado_sprintf(value, " size2i:%d, %d",  ((size2i*)d)->width, ((size2i*)d)->height);
				d += sizeof(size2i);
			}
			break;
			case tornado::class_property_type_rect2:
			{
				tyran_value* x = tyran_value_object_lookup(v, (tyran_object_key*)tyran_string_from_c_str("x"), &dummy_flag);
				((rect2*)d)->x = (number) x->data.number;

				tyran_value* y = tyran_value_object_lookup(v, (tyran_object_key*)tyran_string_from_c_str("y"), &dummy_flag);
				((rect2*)d)->y = (number) y->data.number;

				tyran_value* width = tyran_value_object_lookup(v, (tyran_object_key*)tyran_string_from_c_str("width"), &dummy_flag);
				((rect2*)d)->width = (number) width->data.number;

				tyran_value* height = tyran_value_object_lookup(v, (tyran_object_key*)tyran_string_from_c_str("height"), &dummy_flag);
				((rect2*)d)->height = (number) height->data.number;

				tornado_sprintf(value, " rect2:%f, %f", ((rect2*)d)->width, ((rect2*)d)->height);

				d += sizeof(rect2);
			}
			break;
			case tornado::class_property_type_rotation:
			{
				tyran_value* x = tyran_value_object_lookup(v, (tyran_object_key*)tyran_string_from_c_str("x"), &dummy_flag);
				float yaw = (float) x->data.number;

				tyran_value* y = tyran_value_object_lookup(v, (tyran_object_key*)tyran_string_from_c_str("y"), &dummy_flag);
				float pitch = (float) y->data.number;

				tyran_value* z = tyran_value_object_lookup(v, (tyran_object_key*)tyran_string_from_c_str("z"), &dummy_flag);
				float roll = (float) z->data.number;


				quaternion q;

				quaternion_from_euler(&q, yaw, pitch, roll);

				*((quaternion*)d) = q;

				tornado_sprintf(value, " rotation:%f, %f, %f", yaw, pitch, roll);

				d += sizeof(quaternion);
			}

			break;
			case tornado::class_property_type_callback_function:
			{
				TORNADO_ERROR("TO BE IMPLEMENTED");
			}
			break;
		}
		
		// TORNADO_C_LOG(" Name:%s tyran_value:%s ", name, value);
	}

	const int alignment = 8;

	if (((intptr_t)d % alignment) != 0) {
		d += alignment - ((intptr_t)d % alignment);
	}

	return (d-buf);
}

*/

int e;
