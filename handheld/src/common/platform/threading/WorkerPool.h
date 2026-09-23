enum class WorkerRole
{
	Streaming,
	Disk,
	MainThread,
};



class BackgroundWorker
{
public:

	static bool NOP()
	{
		return true;
	}

	template<typename F>
	void queue(F&& task)
	{
		task();
	}

	void sync()
	{

	}

	void flush()
	{

	}

	template<typename F, typename F2>
	void queue(F&& task, F2&& task2, int priority)
	{
		task(); task2();
	}


};

class WorkerPool
{
public:

	static std::vector<BackgroundWorker*> getWorkersFor(WorkerRole role)
	{
		return std::vector<BackgroundWorker*>();
	}

	static BackgroundWorker& getFor(WorkerRole role)
	{
		static BackgroundWorker worker;//wont matter as this is single threaded now
		return worker;

	}

	static std::unique_ptr<WorkerPool> mInstance;

};

