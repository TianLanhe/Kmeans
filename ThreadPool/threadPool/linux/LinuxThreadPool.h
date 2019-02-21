#ifndef LINUX_THREAD_POOL_H
#define LINUX_THREAD_POOL_H

#include "../../include/ThreadPool.h"

#include <pthread.h>
#include <vector>
#include <queue>

#define MAX_THREAD_SIZE 100

class Thread;

class LinuxThreadPool : public ThreadPool
{
public:
    LinuxThreadPool(int threadsNum = 1, int maxThreadsNum = MAX_THREAD_SIZE);
    ~LinuxThreadPool();
    
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
    	m_mutex = new pthread_mutex_t;
        pthread_mutex_init(m_mutex,NULL);
    }
    
    void _destroyMutex(){
        pthread_mutex_destroy(m_mutex);
        delete m_mutex;
        m_mutex = NULL;
    }
    
    void _lock(){
        pthread_mutex_lock(m_mutex);
    }
    
    void _unlock(){
        pthread_mutex_unlock(m_mutex);
    }
    
    std::vector<Thread*> m_threads;
    std::queue<Task*> m_tasks;
    
    int m_maxThreadsNum;
    int m_threadsNum;
    int m_workingThreadsNum;
    
    bool m_bRun;
    
    pthread_mutex_t* m_mutex;
};

#endif
