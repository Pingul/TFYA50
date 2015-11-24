#ifndef THREADPOOL
#define THREADPOOL

#include <queue>
#include <vector>
#include <thread>
#include <mutex>

#include <iostream>

class Task
{
public:
	Task() = default;
	virtual ~Task() = default;

	virtual void execute() = 0;
};

class tTask : public Task
{
public:
	tTask() = default;
	~tTask() = default;

	void execute() { std::cout << "hello world!" << std::endl; }
};

class Threadpool
{
public:
	Threadpool(int n);
	~Threadpool();

	void addTask(Task*);
	void sync(); // waits till the whole task queue has been run

private:
	std::mutex taskQueueLock;
	std::queue<Task*> tasks;
	std::vector<std::thread> threads;
	bool stopThreads{false};

	void taskLoop();
};

#endif