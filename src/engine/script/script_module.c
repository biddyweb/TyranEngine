#include "script_module.h"

#include <tyran_engine/resource/id.h>
#include "event/script_object_updated.h"
#include <tyran_engine/event/resource_updated.h>

#include "../state/state_update.h"
#include <tyran_engine/module/modules.h>
#include <tyranscript/tyran_log.h>
#include <tyranscript/tyran_memory.h>
#include <tyran_engine/event/resource_load.h>
#include <mocha/parser.h>


extern nimbus_modules* g_modules;

MOCHA_FUNCTION(load_state)
{
    nimbus_script_module* self = runtime->user_data;
    
    const mocha_keyword* resource_keyword = &arguments->objects[1]->data.keyword;
    const char* resource_string = mocha_string_to_c(resource_keyword->string);
    
    nimbus_resource_id resource_id = nimbus_resource_id_from_string(resource_string);
    nimbus_resource_type_id resource_type_id = nimbus_resource_type_id_from_string("mocha");

    nimbus_resource_load_send(&self->update.event_write_stream, resource_id, resource_type_id);
    return mocha_values_create_nil(runtime->values);
}

static void _on_update(void* _self)
{
	// nimbus_script_module* self = _self;

	// nimbus_state_update_send(&self->main_state.arrays, &self->update.event_write_stream);
}

static void boot_script(nimbus_script_module* self)
{
    mocha_runtime_init(&self->runtime);
    self->runtime.user_data = self;
    mocha_runtime_add_function(&self->runtime, "load_state", load_state);
}

static void _on_resource_updated(void* _self, struct nimbus_event_read_stream* stream)
{
	nimbus_script_module* self = _self;
	nimbus_resource_updated* updated;

	nimbus_event_stream_read_type_pointer(stream, updated, nimbus_resource_updated);
	if (updated->resource_type_id == self->script_type_id) {
        mocha_parser parser;
        
        mocha_string script;
        mocha_string_init_from_c(&script, (const char*)stream->pointer);
        
        TYRAN_LOG("read '%s'", mocha_string_to_c(&script));

        mocha_parser_init(&parser, self->runtime.context, script.string, script.count);
        mocha_error error;
        mocha_error_init(&error);
        const mocha_object* result = mocha_parser_parse(&parser, &error);

        mocha_error_init(&error);
        mocha_runtime_eval_commands(&self->runtime, result, &error);
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

	boot_script(self);
    

	self->script_type_id = nimbus_resource_type_id_from_string("mocha");
	nimbus_event_listener_listen(&self->update.event_listener, NIMBUS_EVENT_RESOURCE_UPDATED, _on_resource_updated);
}
