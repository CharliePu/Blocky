#pragma once
#include <thread>

class RThread
{
public:
	void start(void func());
	void stop();
private:
	void run(void func());
	bool threadShouldClose;
	std::thread rThread;
};