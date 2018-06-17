#include "Debug.h"

void sendError(const std::string &description)
{
	std::cerr << description << std::endl;
}

void glfwErrorCallback(int error, const char * description)
{
	std::cerr << error << ":" << description << std::endl;
}

void timer(std::string s)
{
	static bool count = false;
	if (!count)
	{
		glfwSetTime(0);
		count = true;
	}
	else {
		std::cerr << s + ": "<< glfwGetTime() << std::endl;
		count = false;
	}
}
