//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class BlendStates
{
public:
	explicit BlendStates()
	{
		states.resize(static_cast<size_t>(BlendStateType::NUM));

		states[static_cast<size_t>(BlendStateType::NORMAL)] = make_shared<BlendState>(false);
		states[static_cast<size_t>(BlendStateType::ALPHA_BLEND)] = make_shared<BlendState>(true);

		GetBlendState(BlendStateType::NORMAL).BindBlendState();
	}

	BlendState& GetBlendState(BlendStateType state)
	{
		auto& statePtr = states[static_cast<size_t>(state)];
		assert(statePtr);
		return *statePtr;
	}

private:
	vector<shared_ptr<BlendState>> states;
};
