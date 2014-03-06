//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class PerfClock 
{
public:
	typedef std::chrono::nanoseconds duration;
	typedef duration::rep rep;
	typedef duration::period period;
	typedef std::chrono::time_point<PerfClock, duration> time_point;

	explicit PerfClock(System& _sys)
		:sys(_sys)
	{
		frequency = sys.PerformanceFrequency();
		baseTime = Now();
	}

	void operator=(const PerfClock&) = delete;

	time_point Now()
	{
		int64 counter = sys.PerformanceCounter();
		return time_point(duration(static_cast<rep>(static_cast<double>(counter) / frequency *
			period::den / period::num)));
	}

	duration TimeSinceStart() 
	{ 
		return Now() - baseTime; 
	}

private:
	System& sys;
	int64 frequency;
	time_point baseTime;
};
