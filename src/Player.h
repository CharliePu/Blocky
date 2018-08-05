#pragma once
#include <math.h>

#include "World.h"
#include "GLCamera.h"


class Player :
	public GLCamera
{
public:
	Player() : GLCamera::GLCamera(){}
	bool collide(PositionVec & pos, const DirectionVec & displace);

	static constexpr float height = 1.8f;
	static constexpr float width = 0.4f;
	static constexpr float length = 0.4f;
private:
	void updateKeyCallback(GLFWwindow * window);
};