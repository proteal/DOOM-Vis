//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class FrameDataWriter
{
public:
	FrameDataBuffer& BeginWriting()
	{
		FrameDataBuffer& buf = buffers[writeIdx];

		buf.segs.clear();
		buf.nodes.clear();

		return buf;
	}

	void EndWriting()
	{
		lock_guard<mutex> lock(mut);
		//DebugLog(L"EndFrame ", chrono::duration_cast<chrono::milliseconds>(PerfClock::Now() - PerfClock::BaseTime()).count(), '\n');

		lastWrittenIdx = writeIdx;
		for (uint i = 0; i < NUM_BUFFERS; i++)
		{
			if (i != readIdx && i != lastWrittenIdx)
			{
				writeIdx = i;
				break;
			}
		}
	}

protected:
	static const uint NUM_BUFFERS = 3;

	FrameDataWriter()
		:writeIdx(0)
		, readIdx(1)
		, lastWrittenIdx(2)
	{
		for (uint i = 0; i < NUM_BUFFERS; i++)
		{
			FrameDataBuffer& buf = buffers[i];

			buf.pixels.fill({ 0 });
			buf.viewPos = { 0.0f, 0.0f, 0.0f };
			buf.viewDir = { 0.0f, 1.0f, 0.0f };
			buf.playerAngle = 0.0f;
			buf.playerPos = { 0.0f, 0.0f, 0.0f };
		}
	}

	array<FrameDataBuffer, NUM_BUFFERS> buffers;
	uint writeIdx;
	uint readIdx;
	uint lastWrittenIdx;
	mutex mut;
};

class FrameData final : public FrameDataWriter
{
public:
	FrameData()
		:FrameDataWriter()
	{
	}

	const FrameDataBuffer& BeginReading()
	{
		{
			lock_guard<mutex> lock(mut);
			readIdx = lastWrittenIdx;
			//DebugLog(L"Read ", chrono::duration_cast<chrono::milliseconds>(PerfClock::Now() - PerfClock::BaseTime()).count(), '\n');
		}

		return buffers[readIdx];
	}
};
