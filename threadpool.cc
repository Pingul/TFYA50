#include "threadpool.h"

Threadpool* Threadpool::_shared = nullptr;

Threadpool::Threadpool(int n)
{
	nbr_threads = n;
	threadBusy = new bool[n];
	for (int i = 0; i < n; i++)
	{
		threads.push_back(std::thread{&Threadpool::taskLoop, this, i});
	}
}

Threadpool::~Threadpool()
{
	// finish tasks
	sync();
	stopThreads = true;
	for (auto& thread : threads)
	{
		thread.join();
	}
}

void Threadpool::taskLoop(int n)
{
	while (!stopThreads)
	{
		Task* task{nullptr};
		{
			std::lock_guard<std::mutex> lock(taskQueueLock);
			if (!tasks.empty())
			{
				task = tasks.front();
				tasks.pop();
				threadBusy[n] = true;
			}
			// Lock released
		}
		if (task != nullptr)
		{
			task->execute();
			threadBusy[n] = false;
		}
	}
}

void Threadpool::addTask(Task* t)
{
	std::lock_guard<std::mutex> lock(taskQueueLock);
	tasks.push(t);
}

void Threadpool::sync()
{
	while(!tasks.empty())
	{
		std::this_thread::yield();
	}

	bool busy;
	do
	{
		busy = false;
		for (int i = 0; i < nbr_threads; i++)
			busy |= threadBusy[i];
	} while(busy);
}

void Threadpool::dispatchOnce(int n)
{
	if (_shared == nullptr)
		_shared = new Threadpool(n);
}

// int main()
// {
// 	Threadpool::dispatchOnce(4);
// 	tTask t1;
// 	bTask t2;
// 	Threadpool::shared().addTask(&t1);
// 	Threadpool::shared().addTask(&t1);
// 	Threadpool::shared().addTask(&t1);
// 	Threadpool::shared().addTask(&t1);
// 	Threadpool::shared().addTask(&t2);
// 	Threadpool::shared().sync();
// 	Threadpool::shared().addTask(&t2);
// 	Threadpool::shared().addTask(&t2);
// 	Threadpool::shared().addTask(&t2);
// 	Threadpool::shared().addTask(&t2);
// 	Threadpool::shared().addTask(&t1);
// 	Threadpool::shared().sync();

// }