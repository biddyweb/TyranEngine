#include "nacl_input.h"

#include <tyranscript/tyran_log.h>

#include <tyran_core/platform/nacl/nacl.h>
#include <tyran_engine/event/touch_changed.h>

static void _on_update(void* self)
{
}

static void send_touch_changed(nimbus_nacl_input* self, u8t type_id, nimbus_vector2 position)
{
	nimbus_touch_changed_send(&self->update.event_write_stream, type_id, position);
}

static void handle_wheel_input_event(nimbus_nacl_input* self, PP_InputEvent_Type input_event_type, struct PP_FloatPoint deltaPosition)
{
	nimbus_vector2 vector;
	vector.x = deltaPosition.x;
	vector.y = deltaPosition.y;
	send_touch_changed(self, NIMBUS_EVENT_TOUCH_ZOOM_ID, vector);
}

static void handle_mouse_button(nimbus_nacl_input* self, PP_InputEvent_Type input_event_type, struct PP_Point position)
{
	nimbus_event_type_id type_id = 0;
	switch (input_event_type) {
		case PP_INPUTEVENT_TYPE_MOUSEDOWN:
			self->is_mouse_down = TYRAN_TRUE;
			type_id = NIMBUS_EVENT_TOUCH_BEGAN_ID;
			break;
		case PP_INPUTEVENT_TYPE_MOUSEUP:
			self->is_mouse_down = TYRAN_FALSE;
			type_id = NIMBUS_EVENT_TOUCH_ENDED_ID;
			break;
		case PP_INPUTEVENT_TYPE_MOUSEMOVE:
			if (self->is_mouse_down) {
				type_id = NIMBUS_EVENT_TOUCH_MOVED_ID;
			} else {
				type_id = NIMBUS_EVENT_TOUCH_HOVER_ID;
			}
			break;
		default:
			// TYRAN_LOG("Unknown?");
			break;
	}

	if (type_id) {
		nimbus_vector2 vector;
		vector.x = position.x;
		vector.y = -position.y;
		send_touch_changed(self, type_id, vector);
	}
}

static void handle_mouse_input_event(nimbus_nacl_input* self, PP_Resource input_event, PP_InputEvent_Type input_event_type, struct PP_Point position)
{
	handle_mouse_button(self, input_event_type, position);
}

static void handle_keyboard_input_event(nimbus_nacl_input* self, PP_InputEvent_Type input_event_type, uint32_t keycode)
{
	/*
		case PP_INPUTEVENT_TYPE_KEYDOWN:
			TYRAN_LOG("KeyDown");
			break;
		case PP_INPUTEVENT_TYPE_KEYUP:
			TYRAN_LOG("KeyUp");
			break;
	*/

}


static PP_Bool on_input_event(nimbus_nacl_input* self, PP_Resource input_event, PP_InputEvent_Type input_event_type)
{
	if (self->wheel_interface->IsWheelInputEvent(input_event)) {
		struct PP_FloatPoint value = self->wheel_interface->GetDelta(input_event);
		handle_wheel_input_event(self, input_event_type, value);
	} else if (self->mouse_interface->IsMouseInputEvent(input_event)) {
		struct PP_Point point = self->mouse_interface->GetPosition(input_event);
		handle_mouse_input_event(self, input_event, input_event_type, point);
	} else if (self->keyboard_interface->IsKeyboardInputEvent(input_event)) {
		uint32_t keycode = self->keyboard_interface->GetKeyCode(input_event);
		handle_keyboard_input_event(self, input_event_type, keycode);
	} else {
		TYRAN_LOG("Unknown event!");
	}

	return PP_TRUE;
}

static PP_Bool _on_input_event(PP_Instance instance, PP_Resource input_event)
{
	nimbus_nacl_input* self = g_nacl.input_event_receiver_self;

	PP_InputEvent_Type input_event_type = g_nacl.input_event->GetType(input_event);
	/*
		uint32_t modifiers = g_nacl.input_event->GetModifiers(input_event);
		PP_INPUTEVENT_MODIFIER_SHIFTKEY
		PP_INPUTEVENT_MODIFIER_CONTROLKEY
		PP_INPUTEVENT_MODIFIER_ALTKEY
	*/

	return on_input_event(self, input_event, input_event_type);
}

void nimbus_nacl_input_init(void* _self, tyran_memory* memory)
{
	nimbus_nacl_input* self = _self;
	nimbus_update_init(&self->update, memory, _on_update, self, "nacl_input");

	self->mouse_interface = (PPB_MouseInputEvent*) g_nacl.get_browser(PPB_MOUSE_INPUT_EVENT_INTERFACE);
	self->keyboard_interface = (PPB_KeyboardInputEvent*) g_nacl.get_browser(PPB_KEYBOARD_INPUT_EVENT_INTERFACE);
	self->wheel_interface = (PPB_WheelInputEvent*) g_nacl.get_browser(PPB_WHEEL_INPUT_EVENT_INTERFACE);
	self->is_mouse_down = TYRAN_FALSE;

	g_nacl.input_event_receiver = _on_input_event;
	g_nacl.input_event_receiver_self = _self;
}