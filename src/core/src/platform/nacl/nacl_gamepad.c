#include "nacl_gamepad.h"

#include <tyranscript/tyran_log.h>

#include <tyran_core/platform/nacl/nacl.h>
#include <tyran_engine/event/touch_changed.h>

#include <tyran_core/platform/nacl/nacl.h>

static void compare_states(nimbus_nacl_gamepad* self, const struct PP_GamepadsSampleData* new_gamepads)
{
	static struct PP_GamepadsSampleData old_gamepads;

	for (uint32_t gamepad_index = 0; gamepad_index < new_gamepads->length; ++gamepad_index) {
		const struct PP_GamepadSampleData* new_data = &new_gamepads->items[gamepad_index];
		const struct PP_GamepadSampleData* old_data = &old_gamepads.items[gamepad_index];

		for (uint32_t button_index = 0; button_index < new_data->buttons_length; ++button_index) {
			if (new_data->buttons[button_index] != old_data->buttons[button_index]) {
				TYRAN_LOG("Gamepad(%d), Button(%d) v:%f", gamepad_index, button_index, new_data->buttons[button_index]);
			}
		}

		for (uint32_t axes_index = 0; axes_index < new_data->axes_length; ++axes_index) {
			if (new_data->axes[axes_index] != old_data->axes[axes_index]) {
				TYRAN_LOG("Gamepad(%d), Axis(%d) v:%f", gamepad_index, axes_index, new_data->axes[axes_index]);
			}
		}
	}

	old_gamepads = *new_gamepads;
}

static void _on_update(void* _self)
{
	nimbus_nacl_gamepad* self = _self;

	struct PP_GamepadsSampleData gamepads;
	self->gamepad_interface->Sample(g_nacl.module_instance, &gamepads);
	compare_states(self, &gamepads);
}

void nimbus_nacl_gamepad_init(void* _self, struct tyran_memory* memory)
{
	nimbus_nacl_gamepad* self = _self;
	nimbus_update_init(&self->update, memory, _on_update, self, "nacl_gamepad");

	self->gamepad_interface = (PPB_Gamepad*) g_nacl.get_browser(PPB_GAMEPAD_INTERFACE);
}
