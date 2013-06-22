/*
void add_reference(resource_loading_job& info, tyran_value* combine, tyran_value* target, const std::string& reference)
{
	reference_type* t = &info.references[info.reference_count];
	info.reference_count++;
	t->reference = reference;
	t->combine = combine;
	t->target = target;
}

void add_resource_reference(resource_loading_job& info, tyran_value* target, resource_id_type id)
{
	resource_reference_type* t = &info.resource_references[info.resource_reference_count];
	info.resource_reference_count++;
	t->id = id;
	t->target = target;
	
	load_resource_if_needed(info, id);
}

void request_inherits_and_references(resource_loading_job& info, tyran_value* combine, tyran_value* o)
{
	// tyran_print_value("Checking out the inherits", o, 1);
	tyran_value iterator_value;
	tyran_value_object_fetch_key_iterator(runtime, o, &iterator_value);

	tyran_object_iterator *io = iterator_value.data.object->data.iterator;
	for (int key_index = 0; key_index < io->count; ++key_index)
	{
		const tyran_object_key* key = io->keys[key_index];
		tyran_object_key_flag_type flag = 0;
		tyran_value* value = tyran_value_object_lookup_prototype(o, key, &flag);
		char c_key[128];
		tyran_string_to_c_str(c_key, 128, key);
		TORNADO_LOG("key:" << c_key);
		if (value->type == TYRAN_VALUE_TYPE_STRING)
		{
			tyran_string_to_c_str(c_key, 128, value->data.str);
			const char* svalue = c_key;
			if (svalue[0] == '@')
			{
				std::string resource_name(&svalue[1]);
				// TORNADO_LOG("found a resource reference to '" << resource_name);
				resource_id_type id = resource_id(resource_name.c_str()).serializable_id();
				tyran_value* resource = find_resource(id);
				if (resource != 0)
				{
					tyran_value_release(*value);
					tyran_value_copy(*value, *resource);
				}
				else
				{
					TORNADO_LOG("RESOURCE REFERENCE:" << resource_name);

					add_resource_reference(info, value, id);
				}
			}
			else if (svalue[0] == '#')
			{
				std::string reference(&svalue[1]);
				// TORNADO_LOG("found a reference to '" << reference);
				add_reference(info, combine, value, reference);
			}
			else if (tyran_string_strcmp(key, tyran_string_from_c_str("inherit")) == 0)
			{
				tyran_string_to_c_str(c_key, 128, value->data.str);
				const char* class_name = c_key;
				TORNADO_LOG("Found inherit:'" << class_name << "'");
				inherit_resource(info, o, resource_id(class_name).serializable_id());
				tyran_object_delete(o->data.object, key);
			}
		}
		else if (value->type == TYRAN_VALUE_TYPE_OBJECT)
		{
			if (tyran_value_is_function(value))
			{
			}
			else
			{
				request_inherits_and_references(info, combine, value);
			}
		}
	}
}

void resolve_reference_stack(resource_loading_job& info)
{
	for (uint i = 0; i < info.reference_count; ++i)
	{
		reference_type* t = &info.references[i];
		const tyran_string* uni_string = tyran_string_from_c_str(t->reference.c_str());
		const tyran_object_key* key = tyran_object_key_new(uni_string, 0);

		tyran_object_key_flag_type flag;
		tyran_value* value = tyran_value_object_lookup_prototype(t->combine, key, &flag);
		if (value == 0) {
			TORNADO_ERROR("Couldn't find reference: '" << t->reference << "'");
		}		
		tyran_value_copy(*t->target, *value);
	}
	info.reference_count = 0;
}

void resolve_resource_reference_stack(resource_loading_job& info)
{
	for (uint i = 0; i < info.resource_reference_count; ++i)
	{
		resource_reference_type* t = &info.resource_references[i];
		tyran_value* resource = get_resource(t->id);
		tyran_value_release(*t->target);
		tyran_value_copy(*t->target, *resource);
	}
	info.resource_reference_count = 0;
}


void inherit_resource(resource_loading_job& info, tyran_value* target, resource_id_type resource_id_value)
{
	load_resource_if_needed(info, resource_id_value);
	TORNADO_ASSERT(info.inherit_count < 100, "Too many inherits");
	info.inherits[info.inherit_count].resource_value_id = resource_id_value;
	tyran_value* copy = tyran_value_duplicate(target);
	info.inherits[info.inherit_count].target = copy;
	info.inherit_count++;
}

void resolve_inherit_stack(resource_loading_job& info)
{
	for (int i = info.inherit_count - 1; i >= 0; --i)
	{
		inherit_type* t = &info.inherits[i];
		TORNADO_LOG("#" << i << " Setting reference from #" << t->resource_value_id);
		tyran_value* base = get_resource(t->resource_value_id);

		// tyran_value* v = tyran_value_new();
		//tyran_value_set_object(*v, tyran_object_new());
	//	quadwheel_clone(v, base);
		tyran_value_object_set_prototype(t->target, base);
	}

	info.inherit_count = 0;
}

tyran_value* get_resource(resource_id_type resource_id_value)
{
	tyran_value* v = find_resource(resource_id_value);
	TORNADO_ASSERT(v != 0, "get_resource requires an existing resource:" << resource_id_value);
	
	return v;
}

tyran_value* find_resource(resource_id_type resource_id_value)
{
	std::map<resource_id_type, tyran_value*>::const_iterator it = resources.find(resource_id_value);
	if (it == resources.end())
	{
		return 0;
	}

	return it->second;
}

bool is_loading_in_progress(resource_id_type id)
{
	for (resource_loading_collection::const_iterator it = resource_loading_jobs.begin(); it != resource_loading_jobs.end(); ++it)
	{
		const resource_loading_job& info = * (*it);
		if (info.target_resource_id == id)
		{
			return true;
		}
	}
	
	return false;
}

quadwheel_engine::resource_loading_job* start_resource_loading_job(const std::string& description, resource_id_type resource_id_value)
{
	TORNADO_LOG("Starting resource loading:" << description << " id:" << resource_id_value);
	TORNADO_ASSERT(!is_loading_in_progress(resource_id_value), "Can't start a loading job for a resource only in loading");
	TORNADO_ASSERT(find_resource(resource_id_value) == 0, "Can't start resource loading job if resource already is ready?");

	resource_loading_job* job = new resource_loading_job;
	job->source_instance_id = 0;
	job->target_resource_id = resource_id_value;
	job->description = description;
	job->should_clone = false;
	job->should_spawn = false;
	tyran_value_set_undefined(job->root_target_value);
	job->reference_count = 0;
	job->resource_reference_count = 0;
	job->inherit_count = 0;
	
	resource_loading_jobs.push_back(job);
	load_resource(*job, resource_id_value);

	return job;
}

void load_resource(resource_loading_job& info, resource_id_type resource_id_value)
{
	TORNADO_LOG("load sub resource " << resource_id_value << " requested by job " << info.description);
	if (resource_id_value != info.target_resource_id)
	{
		info.resources_to_load.push_back(resource_id_value);
	}
	send_resource_request(resource_id_value);
}

void send_resource_request(resource_id_type resource_id_value)
{
	TORNADO_LOG("Sending out a resource request for " << resource_id_value);
	resource_requested e;
	e.resource_id_value = resource_id_value;
	e_dispatcher.send_event(&e, sizeof(e), resource_requested_id, 0);
}

void load_resource_if_needed(resource_loading_job& job, resource_id_type resource_id_value)
{
	tyran_value* resource_value = find_resource(resource_id_value);
	if (!resource_value)
	{
		job.resources_to_load.push_back(resource_id_value);
		if (!is_loading_in_progress(resource_id_value))
		{
			std::ostringstream s;
			s << job.description << " needed " << resource_id_value;
			start_resource_loading_job(s.str(), resource_id_value);
		}
		else
		{
			TORNADO_LOG("LOAD(" << resource_id_value << ") already loading - ignoring request");
		}
	}
	else
	{
		TORNADO_LOG("LOAD(" << resource_id_value << ") already loaded");
	}
}
*/
int a;
