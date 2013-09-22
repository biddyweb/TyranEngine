#ifndef nimbus_nacl_h
#define nimbus_nacl_h

#include <ppapi/c/ppp.h>
#include <ppapi/c/ppp_instance.h>
#include <ppapi/c/ppb_input_event.h>
#include <ppapi/c/ppb_var.h>
#include <ppapi/c/ppb_console.h>

typedef struct nimbus_nacl {
	PPB_GetInterface get_browser;
	PPB_InputEvent* input_event;
	PP_Instance module_instance;
	PPB_Var* var;
	PPB_Console* console;
} nimbus_nacl;

extern nimbus_nacl g_nacl;


#endif
