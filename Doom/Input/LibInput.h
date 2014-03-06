//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

DECL_PIMPL(LibInput)

void EnableCursor(bool showCursor);
unique_ptr<class LibInputListener> MakeInputListener(InputCallbacks& listener);

};
