//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

enum class InputButton
{
	CAM_FORWARD,
	CAM_BACK,
	NUM
};

enum class InputTrigger
{
	RESET_CAM,
	TOGGLE_ANIM,
	NUM
};

struct InputStateBuffer
{
	bool focusDevView;
	array<bool, static_cast<size_t>(InputButton::NUM)> buttons;
	array<bool, static_cast<size_t>(InputTrigger::NUM)> triggers;
	int mouseMoveX;
	int mouseMoveY;

	bool& GetButton(InputButton button)
	{
		return buttons[static_cast<size_t>(button)];
	}

	bool& GetTrigger(InputTrigger trigger)
	{
		return triggers[static_cast<size_t>(trigger)];
	}
};

struct InputDefs
{
	static const uint BUTTON1 = 1 << 0;
	static const uint BUTTON2 = 1 << 1;
	static const uint BUTTON3 = 1 << 2;
	static const uint MOUSE_SPEED = 25;

	static const int FUNCTION_TOGGLE_INPUT = 0;
	static const int FUNCTION_RESET_CAM = 1;
	static const int FUNCTION_TOGGLE_ANIM = 2;

	static int GetMouseFlags(bool leftButton, bool middleButton, bool rightButton)
	{
		int state = 0;

		if (leftButton)
		{
			state |= BUTTON1;
		}
		if (middleButton)
		{
			state |= BUTTON3;
		}
		if (rightButton)
		{
			state |= BUTTON2;
		}

		return state;
	}
};
