//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class StreamBuffers
{
public:
	static const uint NUM_BUFFERS = 2;

	explicit StreamBuffers()
		:lockedEnd(0)
	{
		numLocked = 0;
	}

	void GetQueueState(bool& queueEmpty, bool& queueFull)
	{
		uint curNumLocked = numLocked;

		queueEmpty = curNumLocked == 0;
		queueFull = curNumLocked == NUM_BUFFERS;
	}

	const vector<StreamSample>& AddBuffer(vector<StreamSample>& buffer)
	{
		//DebugLog(L"Lock wait ", chrono::duration_cast<chrono::milliseconds>(PerfClock::Now() - PerfClock::BaseTime()).count(), '\n');
		while (numLocked == NUM_BUFFERS)
		{
			this_thread::sleep_for(chrono::milliseconds(15));
		}
		numLocked++;

		//DebugLog(L"Locked ", chrono::duration_cast<chrono::milliseconds>(PerfClock::Now() - PerfClock::BaseTime()).count(), '\n');

		vector<StreamSample>& dest = buffers[lockedEnd];
		lockedEnd = (lockedEnd + 1) % NUM_BUFFERS;

		dest = move(buffer);
		return dest;
	}

	void ReleaseBuffer()
	{
		numLocked--;
		//DebugLog(L"Unlocked ", chrono::duration_cast<chrono::milliseconds>(PerfClock::Now() - PerfClock::BaseTime()).count(), '\n');

#if USE_LOG
		if (numLocked == 0)
		{
			Global::Get().DebugLog(L"Stream dropped\n");
		}
#endif		
	}

private:
	array<vector<StreamSample>, NUM_BUFFERS> buffers;
	uint lockedEnd;
	atomic_uint numLocked;
};
