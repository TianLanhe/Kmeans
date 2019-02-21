#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "DataStructureBase.h"

class Task;

class ThreadPool {
public:
	virtual ~ThreadPool() {}

	virtual Status StartTasks() = 0;
	virtual Status WaitForThreads() = 0;

	virtual Status AddTask(Task*) = 0;
	virtual int GetTaskCount() = 0;

	virtual int GetMaxThreadsNum() = 0;
	virtual Status SetMaxThreadsNum(int) = 0;

	virtual int GetThreadsNum() = 0;
	virtual Status SetThreadsNum(int) = 0;

	virtual int GetWorkThreadsNum() = 0;

	virtual bool IsRunning() = 0;
};

#endif // !THREAD_POOL_H
