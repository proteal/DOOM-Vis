//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class LibInput::Impl
{
	friend class LibInput;
public:
	explicit Impl(HWND _hwnd)
		:hwnd(_hwnd)
		, listener(nullptr)
	{
	}

	bool HandleInputMessage(UINT message, WPARAM wParam, LPARAM lParam)
	{
		assert(listener);
		return listener->HandleInputMessage(message, wParam, lParam);
	}

private:
	HWND hwnd;
	LibInputListener::Impl* listener;
};

unique_ptr<LibInputListener> LibInput::MakeInputListener(InputCallbacks& listener)
{
	unique_ptr<LibInputListener::Impl> listenerImpl = make_unique<LibInputListener::Impl>(impl->hwnd, listener);
	impl->listener = listenerImpl.get();

	return make_unique<LibInputListener>(listenerImpl);
}

inline void LibInput::EnableCursor(bool showCursor)
{
	ShowCursor(showCursor);

	if (showCursor)
	{
		ClipCursor(nullptr);
	}
	else
	{
		RECT rcClip;
		GetWindowRect(impl->hwnd, &rcClip);

		rcClip.top += InputWin32Defs::CLIP_CURSOR_BORDER;
		rcClip.bottom -= InputWin32Defs::CLIP_CURSOR_BORDER;
		rcClip.left += InputWin32Defs::CLIP_CURSOR_BORDER;
		rcClip.right -= InputWin32Defs::CLIP_CURSOR_BORDER;

		ClipCursor(&rcClip);
	}
}
