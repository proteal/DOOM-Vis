//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class LibInputListener::Impl
{
public:
	explicit Impl(HWND hwnd, InputCallbacks& _listener)
		:listener(_listener)
	{
		RAWINPUTDEVICE rid[1];
		rid[0].usUsagePage = InputWin32Defs::HID_USAGE_PAGE_GENERIC;
		rid[0].usUsage = InputWin32Defs::HID_USAGE_GENERIC_MOUSE;
		rid[0].dwFlags = 0;
		rid[0].hwndTarget = hwnd;

		if (!RegisterRawInputDevices(rid, 1, sizeof(rid[0])))
		{
			THROW("RegisterRawInputDevices failed.");
		}
	}

	bool HandleInputMessage(UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_KEYDOWN:
			OnKey(true, wParam);
			return true;

		case WM_KEYUP:
			OnKey(false, wParam);
			return true;

		case WM_LBUTTONDOWN:
			OnMouseLeft(true);
			return true;

		case WM_LBUTTONUP:
			OnMouseLeft(false);
			return true;

		case WM_MBUTTONDOWN:
			OnMouseMiddle(true);
			return true;

		case WM_MBUTTONUP:
			OnMouseMiddle(false);
			return true;

		case WM_RBUTTONDOWN:
			OnMouseRight(true);
			return true;

		case WM_RBUTTONUP:
			OnMouseRight(false);
			return true;

		case WM_INPUT:
			OnRawInput(lParam);
			return true;
		}

		return false;
	}

private:
	void OnKey(bool pressed, WPARAM wParam)
	{
		if (wParam >= VK_F1 && wParam <= VK_F12)
		{
			listener.OnFunctionKey(pressed, wParam - VK_F1);
			return;
		}

		int keyCode;
		if (InputWin32Defs::GetKeyCode(wParam, keyCode))
		{
			listener.OnKey(pressed, keyCode);
		}
	}

	void OnMouseLeft(bool pressed)
	{
		listener.OnMouseLeft(pressed);
	}

	void OnMouseMiddle(bool pressed)
	{
		listener.OnMouseMiddle(pressed);
	}

	void OnMouseRight(bool pressed)
	{
		listener.OnMouseRight(pressed);
	}

	void OnRawInput(LPARAM lParam)
	{
		BYTE lpb[40];
		UINT dwSize = sizeof(lpb);

		GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));
		RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(lpb);

		if (raw->header.dwType == RIM_TYPEMOUSE)
		{
			RAWMOUSE& rawMouse = raw->data.mouse;
			listener.OnMouseMove(rawMouse.lLastX, rawMouse.lLastY);
		}
	}

	void operator =(const Impl&) = delete;

	InputCallbacks& listener;
};
