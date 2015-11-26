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

class Threadpool
{
public:
	Threadpool(int n);
	~Threadpool();

	void addTask(Task*);
	void sync(); // waits till the whole task queue has been run

	static void dispatchOnce(int n);
	static Threadpool& shared() { return *_shared; }

private:
	int nbr_threads;
	std::mutex taskQueueLock;
	std::queue<Task*> tasks;
	std::vector<std::thread> threads;
	bool* threadBusy;
	bool stopThreads{false};

	void taskLoop(int n);

	static Threadpool* _shared;
};

#endif