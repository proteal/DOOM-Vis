//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

DECL_PIMPL(System)

int64 PerformanceCounter();
int64 PerformanceFrequency();
#if USE_LOG
void SysDebugLog(const wstring& str);
#endif
bool BeginFrame();

};
