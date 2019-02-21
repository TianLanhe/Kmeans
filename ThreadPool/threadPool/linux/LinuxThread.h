#ifndef LINUX_THREAD_H
#define LINUX_THREAD_H

#include "../../include/Thread.h"
#include <pthread.h>

class LinuxThread : public Thread
{
public:
    explicit LinuxThread(ThreadPool*);
    ~LinuxThread();
    
    ThreadPool *GetThreadPool();
    
    Status Create();
    Status Destroy();
    
    Status Start();
    Status Wait();
    Status Terminate();
    
    bool IsRunning();
    
    bool IsEqual(const Thread&);
    
private:
    static void* _ThreadProc(void*);

    bool _shouldTerminate();
    void _suspend();
    void _assume();

    Status _initMutex(pthread_mutex_t*& mutex){
    	mutex = new pthread_mutex_t;
    	return pthread_mutex_init(mutex,NULL) == 0 ? OK : ER;
    }

    Status _destroyMutex(pthread_mutex_t*& mutex){
    	int r = pthread_mutex_destroy(mutex);
    	delete mutex;
    	mutex = NULL;
    	return r == 0 ? OK : ER;
    }

    Status _initCond(pthread_cond_t*& cond){
    	cond = new pthread_cond_t;
    	return pthread_cond_init(cond,NULL) == 0 ? OK : ER;
    }

    Status _destroyCond(pthread_cond_t*& cond){
    	int r = pthread_cond_destroy(cond);
    	delete cond;
    	cond = NULL;
    	return r == 0 ? OK : ER;
    }

    void _notifyAll(pthread_cond_t*& cond){
    	pthread_cond_broadcast(cond);
    }
    
    void _lock(pthread_mutex_t*& mutex){
        pthread_mutex_lock(mutex);
    }
    
    void _unlock(pthread_mutex_t*& mutex){
        pthread_mutex_unlock(mutex);
    }
    
    void _waitForCondition(pthread_cond_t*& cond, pthread_mutex_t*& mutex){
        pthread_cond_wait(cond,mutex);
    }
    
    ThreadPool* m_threadPool;
    pthread_t* m_threadId;
    
    bool m_bRun;
    pthread_mutex_t* m_runMutex;
    pthread_cond_t* m_cond;
    
    bool m_bTerminate;
    pthread_mutex_t* m_terminateMutex;
};

Thread* GGetLinuxThread(ThreadPool* pool);

#endif // !LINUX_THREAD_H
