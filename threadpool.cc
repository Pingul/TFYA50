#include "threadpool.h"

Threadpool::Threadpool(int n)
{
	for (int i = 0; i < n; i++)
	{
		std::cout << "thread " << i << " created" << std::endl;
		threads.push_back(std::thread{&Threadpool::taskLoop, this});
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

void Threadpool::taskLoop()
{
	// std::cout << "hello world" << std::endl;
	while (!stopThreads)
	{
		Task* task{nullptr};
		{
			std::lock_guard<std::mutex> lock(taskQueueLock);
			if (!tasks.empty())
			{
				task = tasks.front();
				tasks.pop();
			}
			// Lock released
		}
		if (task != nullptr)
			task->execute();
	}
}

void Threadpool::addTask(Task* t)
{
	std::lock_guard<std::mutex> lock(taskQueueLock);
	tasks.push(t);
}

void Threadpool::sync()
{
	while(!tasks.empty());
}

int main()
{
	Threadpool p{4};
	tTask t1;
	tTask t2;
	tTask t3;
	tTask t4;
	tTask t5;
	p.addTask(&t1);
	p.addTask(&t2);
	p.addTask(&t3);
	p.addTask(&t4);
	p.addTask(&t5);
	p.sync();
}