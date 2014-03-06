//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class DepthStates
{
public:
	explicit DepthStates()
	{
		states.resize(static_cast<size_t>(DepthStateType::NUM));

		states[static_cast<size_t>(DepthStateType::NORMAL)] = make_shared<DepthState>(true);
		states[static_cast<size_t>(DepthStateType::NO_WRITE)] = make_shared<DepthState>(false);

		GetDepthState(DepthStateType::NORMAL).BindDepthState();
	}

	DepthState& GetDepthState(DepthStateType state)
	{
		auto& statePtr = states[static_cast<size_t>(state)];
		assert(statePtr);
		return *statePtr;
	}

private:
	vector<shared_ptr<DepthState>> states;
};
