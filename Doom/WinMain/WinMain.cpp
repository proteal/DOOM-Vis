#include "../stdafx.h"
//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#include "resource.h"
#include "Win32.h"

Global* Global::inst = nullptr;
RenderGlobal* RenderGlobal::inst = nullptr;
IDoomCallbacks* doomCallbacks = nullptr;
System::Impl* System::Impl::inst = nullptr;

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE /*hPrevInstance*/, _In_ LPTSTR /*lpCmdLine*/, _In_ int nCmdShow)
{
	try
	{
		unique_ptr<System> system = make_unique<System>(make_unique<System::Impl>());
		unique_ptr<Global> global = make_unique<Global>(*system);

		system->GetImpl().RunApp(hInstance, nCmdShow);
	}
	catch (exception& e)
	{
		const char* msg = e.what();

		MessageBoxA(nullptr, msg, "Error", MB_OK | MB_ICONERROR);
		OutputDebugStringA(msg);
		return 1;
	}

	return 0;
}
