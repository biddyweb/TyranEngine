#include "nacl_input.h"

#include <tyranscript/tyran_log.h>

#include <tyran_core/platform/nacl/nacl.h>
#include <tyran_engine/event/touch_changed.h>

#include <tyran_core/platform/nacl/nacl.h>

static void compare_states()
{
	for (int gamepad_index=0; gamepad_index<self->gamepads.length; ++gamepad_index) {
		struct PP_GamepadSampleData* new_data = &self->items[gamepad_index];
		struct PP_GamepadSampleData* old_data = &self->gamepads.items[gamepad_index];
		// int max_axes = 
		// for (int axes_index = 0; axes_index < 
	}
}

static void _on_update(void* _self)
{
	nimbus_nacl_gamepad* self = _self;

	struct PP_GamepadsSampleData gamepads;
	self->gamepad_interface->Sample(g_nacl.module_instance, &gamepads);
	compare_states(self, gamepads);
}

static void send_touch_changed(nimbus_nacl_input* self, u8t type_id, nimbus_vector2 position)
{
	nimbus_touch_changed_send(&self->update.event_write_stream, type_id, position);
}

void nimbus_nacl_gamepad_init(void* _self, tyran_memory* memory)
{
	nimbus_nacl_gamepad* self = _self;
	nimbus_update_init(&self->update, memory, _on_update, self, "nacl_gamepad");

	self->gamepad_interface = (PPB_Gamepad*) g_nacl.get_browser(PPB_GAMEPAD_INTERFACE);
}
