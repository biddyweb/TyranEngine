#ifndef nimbus_nacl_gamepad_h
#define nimbus_nacl_gamepad_h

#include <tyran_core/update/update.h>

#include <ppapi/c/ppb_gamepad.h>

struct tyran_memory;

typedef struct nimbus_nacl_gamepad {
	nimbus_update update;
	PPB_Gamepad* gamepad_interface;
	struct PP_GamepadsSampleData gamepads;
} nimbus_nacl_gamepad;

void nimbus_nacl_gamepad_init(void* self, struct tyran_memory* memory);

#endif
