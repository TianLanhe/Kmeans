#include "../include/ThreadPoolFactory.h"

#if defined(_WIN32) || defined(_WIN64)
    #include "window/WinThreadPool.h"
#elif defined(__linux__) || defined(__APPLE__)
    #include "linux/LinuxThreadPool.h"
#endif

ThreadPoolFactory* ThreadPoolFactory::factory = nullptr;

ThreadPoolFactory* ThreadPoolFactory::GetInstance() {
	if (factory == nullptr)
		factory = new ThreadPoolFactory();
	return factory;
}

ThreadPool* ThreadPoolFactory::GetThreadPool() {
#if defined(_WIN32) || defined(_WIN64)
	return new WinThreadPool();
#elif defined(__linux__) || defined(__APPLE__)
    return new LinuxThreadPool();
#endif
	
    return nullptr;
}
