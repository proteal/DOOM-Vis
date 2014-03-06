//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class DoomExecScope
{
public:
	explicit DoomExecScope(DoomInstance& _doom)
		:doom(_doom)
	{
		doom.StartGame();
	}

	~DoomExecScope()
	{
		doom.InterruptGame();
	}

	void operator =(const DoomExecScope&) = delete;

private:
	DoomInstance& doom;
};
