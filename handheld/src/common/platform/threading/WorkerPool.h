#include <thread>

enum class WorkerRole
{
	Streaming,
	Disk,
	MainThread,
	NumWorkerRoles,
};

#ifndef UNSTABLE_THREADING
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
#else

class BackgroundWorker
{
public:
	std::mutex tmutex;
	WorkerRole role;
	std::thread t;
	std::condition_variable vv;
	std::condition_variable taskFinished;
	bool running = true;
	static void NOP()
	{
		//return true;
	}

	void sync()//todo
	{
	}

	void flush()
	{
		std::unique_lock lock(tmutex);
		taskFinished.wait(lock, [this] {
			return tasks.empty();
			});
	}

	void workerThread()
	{
		while (true)
		{
			Task t;

			{
				std::unique_lock lock(tmutex);

				vv.wait(lock, [this] {
					return !tasks.empty() || !running;
					});

				if (!running && tasks.empty())
					return;

				t = std::move(tasks.back());
				tasks.pop_back();
			}

			//Dont stop the main thread
			if (t.t1())
				t.t2();

			taskFinished.notify_all();
		}
	}

	BackgroundWorker() : t(&BackgroundWorker::workerThread, this)
	{

	}

	~BackgroundWorker() 
	{
		{//need to destroy this lock before the worker is asked to join so it can read its stopped
			std::lock_guard lock(tmutex);
			running = false;
		}

		vv.notify_one();
		t.join();
	}

	//template<typename F, typename F2>
	void queue(std::function<bool()> task, std::function<void()> task2 = NOP, int priority = 0)
	{
		std::lock_guard lock(tmutex);
		tasks.emplace_back(task, task2, priority);
		std::sort(tasks.begin(), tasks.end(),
			[](const Task& a, const Task& b) {
				return a.priority > b.priority;
			});
			vv.notify_one();//notify we have a new task
		//task(); task2();
	}

	int numtasks()
	{
		std::lock_guard lock(tmutex);
		return tasks.size();
	}

	struct Task
	{
		Task() {};
		Task(std::function<bool()>& f, std::function<void()>& f2, int pr)
		{
			t1 = f;
			t2 = f2;
			priority = pr;
		}
		std::function<bool()> t1;
		std::function<void()> t2;
		int priority;
	};

	std::vector<Task> tasks;
};

class WorkerPool
{
public:
	static std::vector<std::vector<BackgroundWorker*>> workers;

	//static BackgroundWorker workers[(unsigned int)WorkerRole::NumWorkerRoles];
	static std::vector<BackgroundWorker*>& getWorkersFor(WorkerRole role)
	{
		return workers[(unsigned int)role];
	}

	static BackgroundWorker& getFor(WorkerRole role)
	{
		//return worker with least tasks
		int min = INT_MAX;
		int minO = -1;
		for (int i = 0; i < workers.at((unsigned int)role).size(); i++)
		{
			int todoTasks = workers.at((unsigned int)role).at(i)->numtasks();
			if (todoTasks < min)
			{
				min = todoTasks;
				minO = i;
			}
		}
		if (minO == -1)
			assert(0);
		else
			return *workers.at((unsigned int)role).at(minO);
	}

	static std::unique_ptr<WorkerPool> mInstance;

};

#endif