#ifndef nimbus_nacl_h
#define nimbus_nacl_h

#include <ppapi/c/ppp.h>
#include <ppapi/c/ppp_instance.h>
#include <ppapi/c/ppb_input_event.h>
#include <ppapi/c/ppb_var.h>
#include <ppapi/c/ppb_console.h>
#include <ppapi/c/ppb_graphics_3d.h>
#include <ppapi/c/ppb_view.h>

#include <ppapi/c/ppp_input_event.h>

#include <tyran_engine/type/size2.h>

typedef struct nimbus_nacl {
	PP_Resource graphics_3d_context;
	PPB_GetInterface get_browser;
	PPB_InputEvent* input_event;
	PP_Instance module_instance;
	PPB_Var* var;
	PPB_Console* console;
	PPB_Graphics3D* graphics_3d;
	PPB_View* view;

	PP_Bool (*input_event_receiver)(PP_Instance instance, PP_Resource input_event);
	void* input_event_receiver_self;
	nimbus_size2 display_size;
} nimbus_nacl;

extern nimbus_nacl g_nacl;

#endif
