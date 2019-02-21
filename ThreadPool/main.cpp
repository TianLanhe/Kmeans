#include "include/ThreadPool.h"
#include "include/ThreadPoolFactory.h"

int main() {
	
	// get the thread pool
	ThreadPool *pool = ThreadPoolFactory::GetInstance()->GetThreadPool();

	// set the number of thread in the thread pool
	pool->SetThreadsNum(4);

	// implement your own tasks and add concrete tasks to the thread pool
	// one thing to be noted is that task will be destructed after running
	// pool->AddTask(new ConcreteTask());

	// start tasks under multi thread 
	pool->StartTasks();
	
	// you can do something else while the thread pool is running

	// wait for the thread pool to complete the tasks
	pool->WaitForThreads();

	return 0;
}

