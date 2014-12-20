#include "script_module.h"

#include "event/script_object_updated.h"
#include "../state/state_update.h"

#include <tyran_engine/resource/id.h>
#include <tyran_engine/event/resource_updated.h>
#include <tyran_engine/event/resource_load.h>
#include <tyran_engine/module/modules.h>
#include <tyran_engine/state/component_definition.h>

#include <tyranscript/tyran_log.h>
#include <tyranscript/tyran_memory.h>

#include <mocha/parser.h>
#include <mocha/print.h>
#include <mocha/core.h>


extern nimbus_modules* g_modules;

MOCHA_FUNCTION(load_state)
{
    nimbus_script_module* self = runtime->user_data;

    const mocha_keyword* resource_keyword = &arguments->objects[1]->data.keyword;
    const char* resource_string = mocha_string_to_c(resource_keyword->string);

    nimbus_resource_id resource_id = nimbus_resource_id_from_string(resource_string);

    nimbus_resource_load_send(&self->update.event_write_stream, resource_id, self->state_type_id);
    return mocha_values_create_nil(runtime->values);
}

static void _on_update(void* _self)
{
	// nimbus_script_module* self = _self;

	// nimbus_state_update_send(&self->main_state.arrays, &self->update.event_write_stream);
}

static void boot_script(nimbus_script_module* self)
{
    mocha_values_init(&self->values);
    mocha_runtime_init(&self->runtime, &self->values);

    mocha_context_init(&self->root_context, 0);
    mocha_runtime_push_context(&self->runtime, &self->root_context);
    self->runtime.user_data = self;

    mocha_core_define_context(&self->root_context, self->runtime.values);

    mocha_string type_string;
    mocha_string_init_from_c(&type_string, "type");
    self->type_keyword = mocha_values_create_keyword(self->runtime.values, type_string.string, type_string.count);
    mocha_runtime_add_function(&self->runtime, "load_state", load_state);
}

static const mocha_object* parse(nimbus_script_module* self, const char* code, mocha_error* error)
{
    mocha_parser parser;

    mocha_string script;
    mocha_string_init_from_c(&script, code);

    TYRAN_LOG("read '%s'", mocha_string_to_c(&script));

    mocha_parser_init(&parser, &self->values, self->runtime.context, script.string, script.count);
    const mocha_object* result = mocha_parser_parse(&parser, error);

    return result;
}

static int load_combine(nimbus_script_module* self, const char* type_name)
{
    char buf[128];
    tyran_strcpy(buf, 128, "gameplay/");
    tyran_strncat(buf, type_name, 128);
    nimbus_resource_id resource_id = nimbus_resource_id_from_string(buf);

    nimbus_resource_load_send(&self->update.event_write_stream, resource_id, self->state_type_id);

    return 0;
}

static int load_state_combines(nimbus_script_module* self, const mocha_object* state)
{
    const mocha_list* list = &state->data.list;
    for (size_t i=0; i<list->count; ++i) {
        const mocha_object* o = list->objects[i];
        mocha_object_type t = o->type;
        if (t != mocha_object_type_map) {
            return -1;
        }
        const mocha_map* map = &o->data.map;
        const mocha_object* value = mocha_map_lookup(map, self->type_keyword);
        if (value->type != mocha_object_type_keyword) {
            return -1;
        }
        const mocha_keyword* value_keyword = &value->data.keyword;
        const char* type_string = mocha_string_to_c(value_keyword->string);
        TYRAN_LOG("Found type '%s'", type_string);
        load_combine(self, type_string);
    }

    return 0;
}

static void _on_resource_updated(void* _self, struct nimbus_event_read_stream* stream)
{
	nimbus_script_module* self = _self;
	nimbus_resource_updated* updated;

	nimbus_event_stream_read_type_pointer(stream, updated, nimbus_resource_updated);
	if (updated->resource_type_id == self->script_type_id || updated->resource_type_id == self->state_type_id) {
        mocha_error error;
        mocha_error_init(&error);
        const mocha_object* result = parse(self, (const char*)stream->pointer, &error);
        if (updated->resource_type_id == self->script_type_id) {
            mocha_error_init(&error);
            mocha_context* context = mocha_context_create(&self->root_context);
            mocha_runtime_clear_contexts(&self->runtime);
            mocha_runtime_push_context(&self->runtime, context);
            mocha_runtime_eval_commands(&self->runtime, result, &error);
        } else {
            TYRAN_LOG("STATE:");
            mocha_print_object_debug(result);
            TYRAN_LOG("---");
            load_state_combines(self, result->data.vector.objects[0]);
        }
    }
}


void nimbus_script_module_init(void* _self, struct tyran_memory* memory)
{
	nimbus_script_module* self = _self;
	self->modules = g_modules;
	self->memory = memory;
	nimbus_update_init(&self->update, memory, _on_update, self, "script module");
	self->script_buffer_size = 16 * 1024;
	self->script_buffer = TYRAN_MEMORY_ALLOC(memory, self->script_buffer_size, "Script buffer");

    self->script_type_id = nimbus_resource_type_id_from_string("yaml");
    self->state_type_id = nimbus_resource_type_id_from_string("state");
	boot_script(self);

    nimbus_component_definition* script_program = nimbus_modules_add_component_definition(self->modules, "script", 254, sizeof(nimbus_script_updated));
    nimbus_component_definition_add_property(script_program, "script", NIMBUS_COMPONENT_DEFINITION_REFERENCE_RESOURCE, offsetof(nimbus_script_updated, script));

	nimbus_event_listener_listen(&self->update.event_listener, NIMBUS_EVENT_RESOURCE_UPDATED, _on_resource_updated);
}
