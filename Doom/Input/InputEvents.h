//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class InputEventsClient
{
public:
	void PopEvents(vector<event_t>& buffer)
	{
		lock_guard<mutex> lock(pendingMutex);
		buffer = move(pending);
	}

protected:
	explicit InputEventsClient()
	{
	}

	vector<event_t> pending;
	mutex pendingMutex;
};

class InputEvents final : public InputEventsClient
{
public:
	explicit InputEvents()
		:InputEventsClient()
	{
	}

	void AddEvent(const event_t& evt)
	{
		lock_guard<mutex> lock(pendingMutex);
		pending.push_back(evt);
	}
};
