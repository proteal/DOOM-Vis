//------------------------------------------------------------
// DOOM port and BSP visualization by Daniel Fetter (2013-14)
//------------------------------------------------------------
#pragma once

class Thread
{
public:
	using EntryFunc = function<void(const atomic_bool&)>;
	using ExitFunc = function<void()>;

	explicit Thread()
		:threadStarted(false)
	{
		endThread = false;
	}

	~Thread()
	{
		EndThread();
	}

	void StartThread(EntryFunc entry, ExitFunc exit)
	{
		assert(entry);
		assert(!threadStarted);

		threadStarted = true;
		exitFunc = exit;

		auto threadFunc = [this, entry]()
		{
			try
			{
				entry(endThread);
			}
			catch (exception&)
			{
				exception_ptr ep = current_exception();
				Global::Get().SetThreadException(ep);
			}
		};

		th = thread(threadFunc);
	}

	void EndThread()
	{
		if (threadStarted)
		{
			endThread = true;

			if (exitFunc)
			{
				exitFunc();
			}

			th.join();

			threadStarted = false;
			endThread = false;
		}
	}

	thread& GetThread()
	{
		return th;
	}

private:
	thread th;
	atomic_bool endThread;
	ExitFunc exitFunc;
	bool threadStarted;
};
