//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class InputCallbacks
{
public:
	void OnFunctionKey(bool pressed, int function)
	{
		if (pressed)
		{
			return;
		}

		if (function == InputDefs::FUNCTION_TOGGLE_INPUT)
		{
			state.ToggleDevFocus();
		}
		else if (function == InputDefs::FUNCTION_RESET_CAM)
		{
			state.SetTrigger(InputTrigger::RESET_CAM);
		}
		else if (function == InputDefs::FUNCTION_TOGGLE_ANIM)
		{
			state.SetTrigger(InputTrigger::TOGGLE_ANIM);
		}
	}

	void OnKey(bool pressed, int keyCode)
	{
		if (HandleDevKey(pressed, keyCode))
		{
			return;
		}

		event_t evt;
		evt.type = pressed ? ev_keydown : ev_keyup;
		evt.data1 = keyCode;

		events.AddEvent(evt);
	}

	void OnMouseMove(int mouseX, int mouseY)
	{
		if (state.DevViewFocused())
		{
			state.OnMouseMove(mouseX, mouseY);
			return;
		}

		mouseX *= InputDefs::MOUSE_SPEED;
		mouseY *= InputDefs::MOUSE_SPEED;

		event_t evt;
		evt.data2 = mouseX;
		evt.data3 = -mouseY;

		if (evt.data2 != 0 || evt.data3 != 0)
		{
			evt.type = ev_mouse;
			evt.data1 = InputDefs::GetMouseFlags(mouseLeftPressed, mouseMiddlePressed, mouseRightPressed);

			events.AddEvent(evt);
		}
	}

	void OnMouseLeft(bool pressed)
	{
		if (state.DevViewFocused())
		{
			return;
		}

		mouseLeftPressed = pressed;
		AddMouseButtonEvent();
	}

	void OnMouseMiddle(bool pressed)
	{
		if (state.DevViewFocused())
		{
			return;
		}

		mouseMiddlePressed = pressed;
		AddMouseButtonEvent();
	}

	void OnMouseRight(bool pressed)
	{
		if (state.DevViewFocused())
		{
			return;
		}

		mouseRightPressed = pressed;
		AddMouseButtonEvent();
	}

protected:
	explicit InputCallbacks()
		:events()
		, mouseLeftPressed(false)
		, mouseMiddlePressed(false)
		, mouseRightPressed(false)
	{
	}

	void AddMouseButtonEvent()
	{
		event_t evt;
		evt.type = ev_mouse;

		evt.data1 = InputDefs::GetMouseFlags(mouseLeftPressed, mouseMiddlePressed, mouseRightPressed);
		evt.data2 = 0;
		evt.data3 = 0;

		events.AddEvent(evt);
	}

	bool HandleDevKey(bool pressed, int keyCode)
	{
		if (state.DevViewFocused())
		{
			if (keyCode == 'w')
			{
				state.SetButton(InputButton::CAM_FORWARD, pressed);
			}
			else if (keyCode == 's')
			{
				state.SetButton(InputButton::CAM_BACK, pressed);
			}
			else if (keyCode == KEY_ESCAPE)
			{
				state.ToggleDevFocus();
				return false;
			}

			return true;
		}

		return false;
	}

	InputEvents events;
	InputState state;
	bool mouseLeftPressed;
	bool mouseMiddlePressed;
	bool mouseRightPressed;
};
