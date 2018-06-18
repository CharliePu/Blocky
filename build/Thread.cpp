#include "Thread.h"

void RThread::start(void func())
{
	threadShouldClose = false;
	rThread = std::thread(func);
	rThread.detach();
}

void RThread::stop()
{
	
}

void RThread::run(void func())
{
	while (!threadShouldClose)
	{
		func();
	}
}
