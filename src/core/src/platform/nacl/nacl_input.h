#ifndef nimbus_nacl_input_h
#define nimbus_nacl_input_h

#include <tyran_core/update/update.h>

#include <ppapi/c/ppb_input_event.h>

struct tyran_memory;

typedef struct nimbus_nacl_input {
	nimbus_update update;
	tyran_boolean is_mouse_down;
	PPB_MouseInputEvent* mouse_interface;
	PPB_WheelInputEvent* wheel_interface;
	PPB_KeyboardInputEvent* keyboard_interface;
} nimbus_nacl_input;

void nimbus_nacl_input_init(void* self, struct tyran_memory* memory);

#endif
