#ifndef THREAD_H
#define THREAD_H

#include "DataStructureBase.h"

class ThreadPool;

class Thread {
public:
	virtual ~Thread() {}

	virtual ThreadPool *GetThreadPool() = 0;

	virtual Status Create() = 0;
	virtual Status Destroy() = 0;

	virtual Status Start() = 0;
	virtual Status Wait() = 0;
	virtual Status Terminate() = 0;

	virtual bool IsRunning() = 0;

	virtual bool IsEqual(const Thread&) = 0;
};

#endif // !THREAD_H

