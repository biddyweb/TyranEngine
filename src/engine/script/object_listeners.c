/*
void quadwheel_engine::check_for_listening_functions_on_object(const tyran_runtime* rt, tyran_value* root, tyran_value* o, int depth)
{
	tyran_value iterator_value;
	tyran_value_object_fetch_key_iterator(rt, o, &iterator_value);
	tyran_object_iterator *io = iterator_value.data.object->data.iterator;

	for (int key_index = 0; key_index < io->count; ++key_index)
	{
		const tyran_object_key* key = io->keys[key_index];
		char c_key[128];
		tyran_string_to_c_str(c_key, 128, key);
		std::string name(c_key);
		tyran_object_key_flag_type flag = 0;
		tyran_value* value = tyran_value_object_lookup_prototype(o, key, &flag);
		
		if (value->type == TYRAN_VALUE_TYPE_OBJECT)
		{
			if (tyran_value_is_function(value))
			{
				if (TYRAN_UNICODE_STRLEN(key) > 2 && key[0] == 'o' && key[1] == 'n')
				{
					tyran_string_to_c_str(c_key, 128, key);
					std::string name(c_key);
					TORNADO_LOG("Found listening function:" << name << " on object " << root);
					// tyran_print_value("listening object", root, 1);
					name = name.substr(2);
					check_listening_function(name, value, root);
				}
			}
			else
			{
				if (depth == 0)
				{
					check_for_listening_functions_on_object(runtime, value, value, depth + 1);
				}
			}
		}
	}
}


void quadwheel_engine::check_for_listening_functions(tyran_value* loading_root)
{
	check_for_listening_functions_on_object(runtime, loading_root, loading_root, 0);
}

void quadwheel_engine::check_listening_function(const std::string& name, tyran_value* value, tyran_value* target)
{
	for (uint i = 0; i < listening_definition_count; ++i)
	{
		if (name == listening_definitions[i]->name)
		{
			tornado::event_type_id event_id = listening_definitions[i]->event_type;

			std::map<tornado::event_type_id, std::list<listening_object*>*>::const_iterator it = event_to_listening_objects.find(event_id);

			std::list<listening_object*>* listening_objects;
			if (it != event_to_listening_objects.end())
			{
				listening_objects = it->second;
			}
			else
			{
				listening_objects = new std::list<listening_object*>();
				event_to_listening_objects[event_id] = listening_objects;
			}

			listening_object* lo = new listening_object();
			lo->function = value->data.object->data.function;
			tyran_value_copy(lo->target, *target);
			listening_objects->push_back(lo);
			return;
		}
	}
}
*/

int d;
