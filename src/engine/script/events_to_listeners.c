/*
tyran_value* quadwheel_engine::create_argument_scope(const tornado::class_definition& definition, tornado::event_stream* e)
{
	return convert_from_event_to_object(runtime, 0, e == 0 ? 0 : e->pointer, 0, definition, 1);
}

void quadwheel_engine::on_all_events(const tornado::event_type_id& id, uint octet_size, tornado::event_stream* e)
{
	std::map<tornado::event_type_id, std::list<listening_object*>*>::const_iterator found_listener;
	found_listener = event_to_listening_objects.find(id);
	if (found_listener == event_to_listening_objects.end()) {
		return;
	}

//	TORNADO_LOG("Someone is listening to this event: " << id);

	tyran_runtime_callbacks callbacks;
	callbacks.assign_callback = script_AssignCallback;


	std::map<tornado::event_type_id, const tornado::class_definition*>::const_iterator found_definition;

	found_definition = event_id_to_definition.find(id);
	TORNADO_ASSERT(found_definition != event_id_to_definition.end(), "Couldn't find event type id:" << id);
	const tornado::class_definition* definition = found_definition->second;

	std::list<listening_object*>* listening_objects = found_listener->second;
	std::list<listening_object*>::const_iterator listening_function;
	for (listening_function = listening_objects->begin(); listening_function != listening_objects->end(); ++listening_function)
	{
		const listening_object* lo = *listening_function;
		tyran_value* arguments = create_argument_scope(*definition, e);
		tyran_scope_set_variable_names(arguments, lo->function->static_function->argument_names);
		call(runtime, &callbacks, lo->function, &lo->target, arguments);
	}
}
*/
int c;
