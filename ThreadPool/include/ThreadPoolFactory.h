#ifndef THREAD_POOL_FACTORY_H
#define THREAD_POOL_FACTORY_H

class ThreadPool;

class ThreadPoolFactory {
public:
	static ThreadPoolFactory* GetInstance();

	ThreadPool* GetThreadPool();

private:
	static ThreadPoolFactory* factory;
};

#endif