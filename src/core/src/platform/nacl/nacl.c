#include <tyran_core/platform/nacl/nacl.h>
#include <ppapi/c/ppp_instance.h>
#include <ppapi/c/ppp.h>
#include <ppapi/c/ppb_instance.h>
#include <ppapi/c/pp_errors.h>
#include <string.h>
#include <ppapi/c/ppp_input_event.h>
#include <tyranscript/tyran_log.h>
#include "../../base/boot/nimbus_boot.h"

nimbus_nacl g_nacl;

typedef struct nimbus_nacl_render {
	PPB_Graphics3D* graphics_3d;
	PPB_Instance* instance;
	nimbus_boot* boot;
} nimbus_nacl_render;

nimbus_nacl_render g_nacl_render;

static void nacl_log(enum tyran_log_type type, const char* string)
{
	struct PP_Var log_string = g_nacl.var->VarFromUtf8(string, tyran_strlen(string));
	g_nacl.console->Log(g_nacl.module_instance, PP_LOGLEVEL_LOG, log_string);
	g_nacl.var->Release(log_string);
}

PP_EXPORT int32_t PPP_InitializeModule(PP_Module a_module_id, PPB_GetInterface get_browser)
{
	g_nacl.console = (PPB_Console*)(get_browser(PPB_CONSOLE_INTERFACE));
	g_nacl.var = (PPB_Var*)(get_browser(PPB_VAR_INTERFACE));
	g_nacl.get_browser = get_browser;
	g_nacl.input_event = (PPB_InputEvent*)(get_browser(PPB_INPUT_EVENT_INTERFACE));

	return PP_OK;
}

PP_EXPORT void PPP_ShutdownModule()
{

}
static PP_Bool InputEvent_HandleInputEvent(PP_Instance instance, PP_Resource input_event)
{
	if (g_nacl.input_event_receiver) {
		return g_nacl.input_event_receiver(instance, input_event);
	} else {
		return PP_FALSE;
	}


	return PP_TRUE;
}


static void initialize_3d(nimbus_nacl_render* self)
{
	int width = 1024;
	int height = 768;

	TYRAN_LOG("Graphics3D");

	self->graphics_3d = (PPB_Graphics3D*)(g_nacl.get_browser(PPB_GRAPHICS_3D_INTERFACE));
	TYRAN_ASSERT(self->graphics_3d, "Couldn't fetch browser direct3d");

	int32_t attributes[] = {
		PP_GRAPHICS3DATTRIB_ALPHA_SIZE, 8,
		PP_GRAPHICS3DATTRIB_WIDTH, width,
		PP_GRAPHICS3DATTRIB_HEIGHT, height,
		PP_GRAPHICS3DATTRIB_NONE
	};

	TYRAN_LOG("Graphics3D::Create");
	g_nacl.graphics_3d_context = self->graphics_3d->Create(g_nacl.module_instance, 0, attributes);
	TYRAN_LOG("Graphics3D::BindGraphics");
	TYRAN_LOG("Instance");
	self->instance = (PPB_Instance*)(g_nacl.get_browser(PPB_INSTANCE_INTERFACE));
	TYRAN_LOG("Instance2");
	self->instance->BindGraphics(g_nacl.module_instance, g_nacl.graphics_3d_context);
	TYRAN_LOG("DONE");
}


static void draw(nimbus_nacl_render* self);

static void draw_callback(void* _self, int32_t result)
{
	nimbus_nacl_render* self = _self;

	nimbus_boot_update(self->boot);
	draw(self);
}

static void draw(nimbus_nacl_render* self)
{
	struct PP_CompletionCallback callback;
	callback.user_data = self;
	callback.func = draw_callback;
	callback.flags = 0;

	self->graphics_3d->SwapBuffers(g_nacl.graphics_3d_context, callback);
}


static PP_Bool Instance_DidCreate(PP_Instance instance, uint32_t argc, const char* argn[], const char* argv[])
{
	g_nacl.module_instance = instance;
	g_log.log = nacl_log;
	g_nacl.input_event_receiver = 0;
	g_nacl.input_event_receiver_self = 0;

	TYRAN_LOG("Instance_DidCreate");

	g_nacl.input_event->RequestInputEvents(instance, PP_INPUTEVENT_CLASS_MOUSE);
	g_nacl.input_event->RequestFilteringInputEvents(instance, PP_INPUTEVENT_CLASS_WHEEL | PP_INPUTEVENT_CLASS_KEYBOARD);

	initialize_3d(&g_nacl_render);

	g_nacl_render.boot = nimbus_boot_new();

	draw(&g_nacl_render);
	/*
	PP_LOGLEVEL_TIP
	PP_LOGLEVEL_LOG
	PP_LOGLEVEL_WARNING
	PP_LOGLEVEL_ERROR
	*/


	return PP_TRUE;
}

static void Instance_DidDestroy(PP_Instance instance)
{
	TYRAN_LOG("Instance_DidDestroy");
}

static void Instance_DidChangeView(PP_Instance instance, PP_Resource view)
{
	TYRAN_LOG("Instance_DidChangeView");
}

static void Instance_DidChangeFocus(PP_Instance instance, PP_Bool has_focus)
{
	TYRAN_LOG("Instance_DidChangeFocus:%d", has_focus);
}

static PP_Bool Instance_HandleDocumentLoad(PP_Instance instance, PP_Resource url_loader)
{
	TYRAN_LOG("Instance_HandleDocumentLoad");
	return PP_TRUE;
}

PP_EXPORT const void* PPP_GetInterface(const char* interface_name)
{
	if (strcmp(interface_name, PPP_INSTANCE_INTERFACE) == 0) {
		static PPP_Instance instance_interface = {
			&Instance_DidCreate,
			&Instance_DidDestroy,
			&Instance_DidChangeView,
			&Instance_DidChangeFocus,
			&Instance_HandleDocumentLoad
		};

		return &instance_interface;
	} else if (strcmp(interface_name, PPP_INPUT_EVENT_INTERFACE) == 0) {
		static PPP_InputEvent instance_interface = {
			&InputEvent_HandleInputEvent
		};

		return &instance_interface;
	}

	return 0;
}
