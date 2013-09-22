#include <tyran_core/platform/nacl/nacl.h>
#include <ppapi/c/ppp_instance.h>
#include <ppapi/c/ppp.h>
#include <ppapi/c/pp_errors.h>
#include <string.h>
#include <ppapi/c/ppp_input_event.h>

nimbus_nacl g_nacl;

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

static void handle_input_event(PP_InputEvent_Type type)
{
	switch (type) {
		case PP_INPUTEVENT_TYPE_MOUSEDOWN:
		break;
		case PP_INPUTEVENT_TYPE_MOUSEUP:
		break;
		case PP_INPUTEVENT_TYPE_MOUSEMOVE:
		break;
		case PP_INPUTEVENT_TYPE_WHEEL:
		break;
		case PP_INPUTEVENT_TYPE_KEYDOWN:
		break;
		case PP_INPUTEVENT_TYPE_KEYUP:
		break;
		default:
		break;
	}

}

static PP_Bool InputEvent_HandleInputEvent(PP_Instance instance, PP_Resource input_event)
{
	PP_InputEvent_Type type = g_nacl.input_event->GetType(input_event);
	
	// uint32_t modifiers = g_nacl.input_event->GetModifiers(input_event);

	handle_input_event(type);
	
	return PP_TRUE;
}


static PP_Bool Instance_DidCreate(PP_Instance instance, uint32_t argc, const char* argn[], const char* argv[])
{
	g_nacl.module_instance = instance;
	g_nacl.input_event->RequestInputEvents(instance, PP_INPUTEVENT_CLASS_MOUSE);
	struct PP_Var log_string = g_nacl.var->VarFromUtf8("Hello, world", 12);
	g_nacl.console->Log(instance, PP_LOGLEVEL_LOG, log_string);
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
}

static void Instance_DidChangeView(PP_Instance instance, PP_Resource view)
{
}

static void Instance_DidChangeFocus(PP_Instance instance, PP_Bool has_focus)
{
}

static PP_Bool Instance_HandleDocumentLoad(PP_Instance instance, PP_Resource url_loader)
{
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
