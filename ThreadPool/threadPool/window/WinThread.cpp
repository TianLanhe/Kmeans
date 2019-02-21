#include "WinThread.h"
#include "WinThreadPool.h"
#include "../../include/Task.h"

Thread* GGetWinThread(ThreadPool* pool) {
	return new WinThread(pool);
}

WinThread::WinThread(ThreadPool* pool) :
	m_threadPool(pool), m_hThread(NULL),
	m_bRun(false), m_bHasWait(true), m_hSemaphore(NULL),
	m_bTerminate(false), m_run_cs(NULL), m_terminate_cs(NULL) {
	Create();
}

WinThread::~WinThread() {
	if (m_hThread)
		Destroy();
}

ThreadPool* WinThread::GetThreadPool() {
	return m_threadPool;
}

Status WinThread::Create() {
	CHECK_ERROR(!m_hThread);

	m_hThread = CreateThread(NULL, 0, _ThreadProc, this, CREATE_SUSPENDED, NULL);
	CHECK_ERROR(m_hThread);

	if (m_hSemaphore)
		::CloseHandle(m_hSemaphore);
	m_hSemaphore = ::CreateSemaphore(NULL, 0, 1, NULL);
	CHECK_ERROR(m_hSemaphore);

	if (m_run_cs)
		_destroyMutex(m_run_cs);
	_initMutex(m_run_cs);

	m_bRun = false;
	m_bHasWait = true;

	if (m_terminate_cs)
		_destroyMutex(m_terminate_cs);
	_initMutex(m_terminate_cs);

	m_bTerminate = false;

	return OK;
}

Status WinThread::Destroy() {
	CHECK_ERROR(m_hThread);

	Terminate();
	m_bRun = false;
	m_bHasWait = true;

	BOOL r = ::CloseHandle(m_hThread);
	m_hThread = NULL;
	CHECK_ERROR(r);

	r = ::CloseHandle(m_hSemaphore);
	m_hSemaphore = NULL;
	CHECK_ERROR(r);

	if (m_run_cs)
		_destroyMutex(m_run_cs);

	if (m_terminate_cs)
		_destroyMutex(m_terminate_cs);

	return OK;
}

Status WinThread::Start() {
	CHECK_ERROR(!m_bRun && !m_bTerminate); // m_bRun和m_bTerminate有不一致的风险

	BEFORE_CHECK_RESULT();
	if (!m_hThread)
		CHECK_RESULT(Create());

	_assume_();
	return OK;
}

Status WinThread::Wait() {
	CHECK_ERROR(m_hThread);

	_lock(m_terminate_cs);
	if (m_bTerminate) {
		_unlock(m_terminate_cs);
		return ER;
	}
	_unlock(m_terminate_cs);

	int ret = WAIT_OBJECT_0;

	_lock(m_run_cs);
	if (m_bRun) {
		_unlock(m_run_cs);

		ret = ::WaitForSingleObject(m_hSemaphore, INFINITE);
		m_bHasWait = true;
	}
	else {
		_unlock(m_run_cs);
		if (!m_bHasWait) {
			::WaitForSingleObject(m_hSemaphore, INFINITE);
			m_bHasWait = true;
		}
	}

	return ret == WAIT_OBJECT_0 ? OK : ER;
}

Status WinThread::Terminate() {
	CHECK_ERROR(m_hThread);

	_lock(m_terminate_cs);
	if (m_bTerminate) {
		_unlock(m_terminate_cs);
		return ER;
	}
	m_bTerminate = true;
	_unlock(m_terminate_cs);

	if (!IsRunning())
		_assume_();

	DWORD ret = WaitForSingleObject(m_hThread, INFINITE);
	CloseHandle(m_hThread);

	return ret == WAIT_OBJECT_0 ? OK : ER;
}

bool WinThread::IsRunning() {
	bool run;
	_lock(m_run_cs);
	run = m_bRun;
	_unlock(m_run_cs);
	return run;
}

bool WinThread::IsEqual(const Thread& thread) {
	const Thread* ptr = &thread;
	const WinThread* winthread = dynamic_cast<const WinThread*>(ptr);
	return winthread && m_hThread && winthread->m_hThread && winthread->m_hThread == m_hThread;
}

bool WinThread::_shouldTerminate() {
	bool terminate;
	_lock(m_terminate_cs);
	terminate = m_bTerminate;
	_unlock(m_terminate_cs);
	return terminate;
}

void  WinThread::_assume_() {
	_lock(m_run_cs);
	if (!m_bRun) {
		m_bRun = true;
		if (::ResumeThread(m_hThread) == -1)
			m_bRun = false;
	}
	_unlock(m_run_cs);
}

void  WinThread::_suspend() {
	_lock(m_run_cs);
	m_bRun = false;
	m_bHasWait = false;
	::ReleaseSemaphore(m_hSemaphore, 1, NULL);
	_unlock(m_run_cs);
	::SuspendThread(m_hThread);
}

DWORD WINAPI WinThread::_ThreadProc(LPVOID lpParameter) {
	WinThread* ptrThis = (WinThread*)lpParameter;
	WinThreadPool* pool;
	Task* task;

	pool = dynamic_cast<WinThreadPool*>(ptrThis->GetThreadPool());
	if (pool == NULL)
		return 1;

	while (1) {
		if (ptrThis->_shouldTerminate())
			break;

		task = pool->GetTask();
		if (task) {
			task->Run();
			delete task;
			task = NULL;
		}
		else {
			ptrThis->_suspend();
		}
	}
	return 0;
}
