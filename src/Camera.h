#pragma once
#include "World.h"
#include "GLCamera.h"


class Camera :
	public GLCamera
{
public:
	Camera() : GLCamera::GLCamera(){}
private:
	void updateKeyCallback(GLFWwindow * window);
};