#ifndef WIN_THREAD_POOL_H
#define WIN_THREAD_POOL_H

#include "../../include/ThreadPool.h"

#include <Windows.h>
#include <vector>
#include <queue>

#define MAX_THREAD_SIZE 100

class Thread;

class WinThreadPool : public ThreadPool
{
public:
	WinThreadPool(int threadsNum = 1, int maxThreadsNum = MAX_THREAD_SIZE);
	~WinThreadPool();

	Status StartTasks();
	Status WaitForThreads();

	Status AddTask(Task*);
	int GetTaskCount();

	bool IsRunning();

	int GetMaxThreadsNum();
	Status SetMaxThreadsNum(int);

	int GetThreadsNum();
	Status SetThreadsNum(int);

	int GetWorkThreadsNum();

public:
	Task* GetTask();

private:
	void _initMutex(){
		InitializeCriticalSection(&m_cs);
	}

	void _destroyMutex() {
		DeleteCriticalSection(&m_cs);
	}

	void _lock() {
		EnterCriticalSection(&m_cs);
	}
	void _unlock() {
		LeaveCriticalSection(&m_cs);
	}

	std::vector<Thread*> m_threads;
	std::queue<Task*> m_tasks;

	int m_maxThreadsNum;
	int m_threadsNum;
	int m_workingThreadsNum;

	bool m_bRun;

	CRITICAL_SECTION m_cs;
};

#endif
