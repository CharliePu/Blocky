#pragma once

#include <math.h>

#include "World.h"
#include "GLCamera.h"

class Player :
	public GLCamera
{
public:

	Player() : GLCamera::GLCamera(PositionVec(0, 150, 0), 20.0f) {}
	bool collide(PositionVec & pos, const DirectionVec & displace);

	static constexpr float height = 1.8f;
	static constexpr float width = 0.4f;
	static constexpr float length = 0.4f;
	static constexpr int selectRadius = 5;

	Block::GlobalPosVec selectPos;
	Block::Face selectFace;
	Block::Type selectBlock;

	void selectUpdate();
	void selectDraw();
	bool putBlockNearFace(const Block::GlobalPosVec & pos, const Block::Face face, const Block::Type type);

private:
	void updateKeyCallback(GLFWwindow * window) override;
	void updateCursorCallback(GLFWwindow * window) override;
	Block::Face getFace(const DirectionVec &point);
};
