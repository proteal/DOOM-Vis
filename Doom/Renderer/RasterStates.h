//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class RasterStates
{
public:
	explicit RasterStates()
	{
		states.resize(static_cast<size_t>(RasterStateType::NUM));

		states[static_cast<size_t>(RasterStateType::NORMAL)] = make_shared<RasterState>(true, false);
		states[static_cast<size_t>(RasterStateType::TWO_SIDE)] = make_shared<RasterState>(false, false);
		states[static_cast<size_t>(RasterStateType::TWO_SIDE_WIREFRAME)] = make_shared<RasterState>(false, true);

		GetRasterState(RasterStateType::NORMAL).BindRasterState();
	}

	RasterState& GetRasterState(RasterStateType state) 
	{ 
		auto& statePtr = states[static_cast<size_t>(state)];
		assert(statePtr);
		return *statePtr;
	}

private:
	vector<shared_ptr<RasterState>> states;
};
