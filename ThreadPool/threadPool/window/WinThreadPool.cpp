#include "WinThreadPool.h"
#include "WinThread.h"
#include "../../include/Task.h"

using namespace std;

WinThreadPool::WinThreadPool(int threadsNum, int maxThreadsNum) :
	m_threadsNum(threadsNum), m_maxThreadsNum(maxThreadsNum),
	m_bRun(false),
	m_workingThreadsNum(0) {

	if (m_threadsNum < 1)
		m_threadsNum = 1;

	if (m_maxThreadsNum < m_threadsNum)
		m_maxThreadsNum = m_threadsNum;

	_initMutex();

	for (int i = 0; i < m_threadsNum; ++i)
		m_threads.push_back(GGetWinThread(this));
}

WinThreadPool::~WinThreadPool() {
	if (!m_threads.empty()) {
		for (vector<Thread*>::size_type i = 0; i < m_threads.size(); ++i)
			delete m_threads[i];
		m_threads.clear();
	}
	while (!m_tasks.empty()) {
		Task *task = m_tasks.front();
		m_tasks.pop();
		delete task;
	}
	_destroyMutex();
}

Status WinThreadPool::StartTasks() {
	CHECK_ERROR(!m_bRun && !m_workingThreadsNum);

	m_bRun = true;
	BEFORE_CHECK_RESULT();
	for (vector<Thread>::size_type i = 0; i < m_threads.size(); ++i) {
		CHECK_RESULT(m_threads[i]->Start());
		++m_workingThreadsNum;
	}
	return OK;
}

Status WinThreadPool::WaitForThreads() {
	CHECK_ERROR(m_bRun && m_workingThreadsNum);

	BEFORE_CHECK_RESULT();
	for (vector<Thread>::size_type i = 0; i < m_threads.size(); ++i) {
		CHECK_RESULT(m_threads[i]->Wait());
		--m_workingThreadsNum;
	}
	m_bRun = false;
	return OK;
}

int WinThreadPool::GetWorkThreadsNum() {
	return m_workingThreadsNum;
}

int WinThreadPool::GetThreadsNum() {
	return m_threadsNum;
}

int WinThreadPool::GetTaskCount() {
	int size;
	_lock();
	size = m_tasks.size();
	_unlock();
	return size;
}

Status WinThreadPool::SetThreadsNum(int n) {
	CHECK_ERROR(!m_bRun && !m_workingThreadsNum);
	CHECK_ERROR(n > 0 && n <= m_maxThreadsNum);

	if (n != m_threadsNum) {
		if (n > m_threadsNum) {
			while (m_threadsNum < n) {
				m_threads.push_back(GGetWinThread(this));
				++m_threadsNum;
			}
		}
		else {
			while (m_threadsNum > n) {
				--m_threadsNum;
				Thread* thread = m_threads.back();
				m_threads.pop_back();
				delete thread;
			}
		}
	}
	return OK;
}

int WinThreadPool::GetMaxThreadsNum() {
	return m_maxThreadsNum;
}

Status WinThreadPool::SetMaxThreadsNum(int n) {
	CHECK_ERROR(!m_bRun && !m_workingThreadsNum);
	CHECK_ERROR(n > 0);

	if (n != m_maxThreadsNum) {
		while (m_threadsNum > n) {
			--m_threadsNum;
			Thread* thread = m_threads.back();
			m_threads.pop_back();
			delete thread;
		}
		m_maxThreadsNum = n;
	}
	return OK;
}

Status WinThreadPool::AddTask(Task* task) {
	if (task) {
		_lock();
		m_tasks.push(task);
		_unlock();
	}
	return OK;
}

Task* WinThreadPool::GetTask() {
	Task *task;
	_lock();
	if (m_tasks.empty())
		task = NULL;
	else {
		task = m_tasks.front();
		m_tasks.pop();
	}
	_unlock();
	return task;
}

bool WinThreadPool::IsRunning() {
	return m_bRun;
}
