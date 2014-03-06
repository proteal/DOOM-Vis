//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class Global
{
public:
	static Global& Get()
	{
		assert(inst);
		return *inst;
	}

	explicit Global(System& _sys)
		:sys(_sys)
		, clock(_sys)
	{
		inst = this;
		haveException = false;

		tr2::sys::wpath curPath = tr2::sys::initial_path<tr2::sys::wpath>();
		dataPath = curPath / tr2::sys::wpath(L"Data/");
	}

	~Global()
	{
		inst = nullptr;
	}

	void CheckThreadException()
	{
		if (haveException)
		{
			lock_guard<mutex> lock(exceptionMutex);
			rethrow_exception(threadException);
		}
	}

	void SetThreadException(exception_ptr& ex)
	{
		{
			lock_guard<mutex> lock(exceptionMutex);
			if (!threadException)
			{
				threadException = ex;
			}
		}

		haveException = true;
	}
	
#if USE_LOG
	template<typename Arg>
	void DebugLog(const Arg& arg)
	{
		wstringstream ss;
		ss << arg;
		sys.SysDebugLog(ss.str());
	}

	template<typename Arg, typename... Rest>
	void DebugLog(const Arg& arg, const Rest&... rest)
	{
		DebugLog(arg);
		DebugLog(rest...);
	}
#endif

	const wstring& GetDataPath() const
	{
		return dataPath;
	}

	PerfClock& GetClock()
	{
		return clock;
	}

	System& GetSys()
	{
		return sys;
	}

private:
	System& sys;
	wstring dataPath;
	PerfClock clock;
	exception_ptr threadException;
	mutex exceptionMutex;
	atomic_bool haveException;
	static Global* inst;
};
