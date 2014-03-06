//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

#include "../App/AppIncludes.h"
#include "../D3d/D3dIncludes.h"
#include "../Xaudio/XAudioIncludes.h"
#include "../Wmidi/WmidiIncludes.h"
#include "../InputWin32/InputWin32Includes.h"

class System::Impl
{
public:
	explicit Impl()
		:inputListener(nullptr)
		, curApp(nullptr)
		, window(nullptr)
	{
		inst = this;

		/*HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
		if (hr != S_OK)
		{
		THROW("CoInitializeEx failed.");
		}*/
	}

	~Impl()
	{
		//CoUninitialize();
		if (window)
		{
			curApp = nullptr;
			inputListener = nullptr;
			DestroyWindow(window);
		}

		inst = nullptr;
	}

	void RunApp(HINSTANCE hInstance, int nCmdShow)
	{
		window = MakeWindow(hInstance, nCmdShow);

		unique_ptr<LibMidi> libMidi = make_unique<LibMidi>(make_unique<LibMidi::Impl>());
		unique_ptr<LibAudio> libAudio = make_unique<LibAudio>(make_unique<LibAudio::Impl>());
		unique_ptr<LibRenderer> libRenderer = make_unique<LibRenderer>(make_unique<LibRenderer::Impl>(window, false));

		unique_ptr<LibInput::Impl> inputImpl = make_unique<LibInput::Impl>(window);
		inputListener = inputImpl.get();
		unique_ptr<LibInput> libInput = make_unique<LibInput>(inputImpl);

		unique_ptr<App> app = make_unique<App>(libMidi, libAudio, libInput, libRenderer);
		curApp = app.get();
		app->RunApp();
	}

private:
	HWND MakeWindow(HINSTANCE hInstance, int nCmdShow)
	{
		WNDCLASSEX wcex;
		ZeroMemory(&wcex, sizeof(WNDCLASSEX));
		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wcex.lpfnWndProc = StaticWindowProc;
		wcex.hInstance = hInstance;
		wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DOOM));
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOWFRAME + 1);
		wcex.lpszClassName = L"MyClass";
		wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SMALL));

		ATOM wndClass = RegisterClassEx(&wcex);
		if (!wndClass)
		{
			THROW("RegisterClassEx failed.");
		}

		RECT rect;
		const float SCALE = 2.5f;

		rect.left = 0;
		rect.top = 0;
		rect.right = static_cast<LONG>(RendererDefs::TOTAL_WIDTH() * SCALE + 0.5f);
		rect.bottom = static_cast<LONG>(RendererDefs::DOOM_HEIGHT * SCALE + 0.5f);

		DWORD style = WS_OVERLAPPEDWINDOW;
		AdjustWindowRect(&rect, style, FALSE);

		HWND window = CreateWindowEx(0, reinterpret_cast<LPWSTR>(wndClass), L"DOOM", style, 50, 50,
			rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, hInstance, nullptr);

		if (!window)
		{
			THROW("CreateWindowEx failed.");
		}

		ShowWindow(window, nCmdShow);
		UpdateWindow(window);

		return window;
	}

	static LRESULT CALLBACK StaticWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		assert(inst);
		return inst->WindowProc(hWnd, message, wParam, lParam);
	}

	LRESULT WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (curApp)
		{
			assert(inputListener);

			if (!inputListener->HandleInputMessage(message, wParam, lParam))
			{
				switch (message)
				{
				case WM_DESTROY:
					window = nullptr;
					PostQuitMessage(0);
					break;

				case WM_SIZE:
					curApp->OnResize();
					return 1;
				}
			}
		}

		return DefWindowProcW(hWnd, message, wParam, lParam);
	}

	LibInput::Impl* inputListener;
	App* curApp;
	HWND window;
	static System::Impl* inst;
};

inline int64 System::PerformanceCounter()
{
	LARGE_INTEGER counter;
#ifdef _DEBUG
	BOOL ret =
#endif
		QueryPerformanceCounter(&counter);

	assert(ret);
	return counter.QuadPart;
}

inline int64 System::PerformanceFrequency()
{
	LARGE_INTEGER frequency;
	if (!QueryPerformanceFrequency(&frequency))
	{
		THROW("QueryPerformanceFrequency failed.");
	}

	return frequency.QuadPart;
}

#if USE_LOG
inline void System::SysDebugLog(const wstring& str)
{
	OutputDebugString(str.c_str());
}
#endif

inline bool System::BeginFrame()
{
	MSG msg = { 0 };
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (msg.message == WM_QUIT)
		{
			return false;
		}
	}

	return true;
}

#if 0
inline void System::SetThreadFixedProcessor(thread& thr)
{
	SetThreadAffinityMask(thr.native_handle(), 1);
}
#endif
