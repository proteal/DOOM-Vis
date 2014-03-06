//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class InputStateClient
{
public:
	void PopState(InputStateBuffer& _buffer)
	{
		lock_guard<mutex> lock(mut);

		_buffer = buffer;
		buffer.triggers.fill(false);
		buffer.mouseMoveX = buffer.mouseMoveY = 0;
	}

protected:
	explicit InputStateClient()
	{
		buffer.focusDevView = false;
		buffer.buttons.fill(false);
		buffer.triggers.fill(false);
		buffer.mouseMoveX = buffer.mouseMoveY = 0;
	}

	InputStateBuffer buffer;
	mutex mut;
};

class InputState final : public InputStateClient
{
public:
	explicit InputState()
		:InputStateClient()
	{
	}

	void ToggleDevFocus()
	{
		lock_guard<mutex> lock(mut);

		buffer.focusDevView = !buffer.focusDevView;
		buffer.buttons.fill(false);
		buffer.triggers.fill(false);
		buffer.mouseMoveX = buffer.mouseMoveY = 0;
	}

	void SetButton(const InputButton button, bool state)
	{
		lock_guard<mutex> lock(mut);

		buffer.GetButton(button) = state;
	}

	void SetTrigger(const InputTrigger trigger)
	{
		lock_guard<mutex> lock(mut);
		buffer.GetTrigger(trigger) = true;
	}

	void OnMouseMove(int moveX, int moveY)
	{
		lock_guard<mutex> lock(mut);

		buffer.mouseMoveX += moveX;
		buffer.mouseMoveY += moveY;
	}

	bool DevViewFocused() const
	{
		return buffer.focusDevView;
	}
};
