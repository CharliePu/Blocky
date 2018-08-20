#pragma once
#include "GLFrameBuffer.hpp"

class Shadow
{
public:
	Shadow();
	void prepare();
	void render();
private:
	GLFrameBuffer fb;
};