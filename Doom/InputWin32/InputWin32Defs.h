//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

struct InputWin32Defs
{
	static const ushort HID_USAGE_PAGE_GENERIC = 0x01;
	static const ushort HID_USAGE_GENERIC_MOUSE = 0x02;
	static const int CLIP_CURSOR_BORDER = 35;

	static bool GetKeyCode(WPARAM wParam, int& keyCode)
	{
		if (wParam == VK_ESCAPE)
		{
			keyCode = KEY_ESCAPE;
		}
		else
		{
			wchar_t ch = static_cast<wchar_t>(wParam);
			ch = static_cast<wchar_t>(tolower(ch));

			if ((ch >= 'a' && ch <= 'z') || ch == ' ' || (ch >= '0' && ch <= '9'))
			{
				keyCode = ch;
			}
			else if (ch == '\r')
			{
				keyCode = KEY_ENTER;
			}
			else if (ch == '\t')
			{
				keyCode = KEY_TAB;
			}
			else
			{
				return false;
			}
		}

		return true;
	}
};
