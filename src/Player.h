#pragma once
#include <math.h>

#include "World.h"
#include "GLCamera.h"

class Player :
	public GLCamera
{
public:
	static constexpr float height = 1.8f;
	static constexpr float width = 0.4f;
	static constexpr float length = 0.4f;
	static constexpr int selectRadius = 5;
#ifdef _DEBUG
	static constexpr double speed = 30;
#else
	static constexpr double speed = 3;
#endif


	Player() : GLCamera::GLCamera(PositionVec(0, 150, 0), speed) {}
	bool collide(PositionVec & pos, const DirectionVec & displace);

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