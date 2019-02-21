#ifndef WIN_THREAD_H
#define WIN_THREAD_H

#include "../../include/Thread.h"
#include <Windows.h>

class WinThread : public Thread
{
public:
	explicit WinThread(ThreadPool*);
	~WinThread();

	ThreadPool *GetThreadPool();

	Status Create();
	Status Destroy();

	Status Start();
	Status Wait();
	Status Terminate();

	bool IsRunning();

	bool IsEqual(const Thread&);	

private:
	static DWORD WINAPI _ThreadProc(LPVOID lpParameter);

    bool _shouldTerminate();
    void _suspend();
	void _assume_();	// _assume 貌似被占用了，居然用不了。。

    Status _initMutex(CRITICAL_SECTION*& cs){
		cs = new CRITICAL_SECTION;
    	InitializeCriticalSection(cs);
    	return OK;
    }

	Status _destroyMutex(CRITICAL_SECTION*& cs) {
		DeleteCriticalSection(cs);
		delete cs;
		cs = NULL;
		return OK;
	}

	void _lock(CRITICAL_SECTION* &cs) {
		EnterCriticalSection(cs);
	}

	void _unlock(CRITICAL_SECTION* &cs) {
		LeaveCriticalSection(cs);
	}

	ThreadPool* m_threadPool;
	HANDLE m_hThread;

	bool m_bHasWait;
	bool m_bRun;
	CRITICAL_SECTION* m_run_cs;
	HANDLE m_hSemaphore;

	bool m_bTerminate;
	CRITICAL_SECTION* m_terminate_cs;
};

Thread* GGetWinThread(ThreadPool* pool);

#endif // !WIN_THREAD_H
