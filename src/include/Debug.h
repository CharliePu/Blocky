#pragma once
#include <iostream>
#include <string>

#include <GLFW\glfw3.h>

void sendError(const std::string & description);

void glfwErrorCallback(int error, const char * description);

void timer(std::string s);