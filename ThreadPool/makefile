DEBUG = -g

all:main

main:main.o LinuxThread.o LinuxThreadPool.o ThreadPoolFactory.o
	g++ -o main *.o -lpthread

main.o:main.cpp include/ThreadPool.h include/ThreadPoolFactory.h
	g++ -c main.cpp $(DEBUG)

LinuxThread.o:threadPool/linux/LinuxThread.cpp threadPool/linux/LinuxThread.h threadPool/linux/LinuxThreadPool.h include/Task.h
	g++ -c threadPool/linux/LinuxThread.cpp $(DEBUG)

LinuxThreadPool.o:threadPool/linux/LinuxThreadPool.cpp threadPool/linux/LinuxThreadPool.h threadPool/linux/LinuxThread.h include/Task.h
	g++ -c threadPool/linux/LinuxThreadPool.cpp -std=c++11 $(DEBUG)

ThreadPoolFactory.o:threadPool/ThreadPoolFactory.cpp include/ThreadPoolFactory.h threadPool/linux/LinuxThread.h
	g++ -c threadPool/ThreadPoolFactory.cpp -std=c++11 $(DEBUG)

clean:
	rm *.o

uninstall:
	rm *.o main
