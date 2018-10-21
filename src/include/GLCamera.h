#pragma once

#include <mutex>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <GLFW\glfw3.h>

class GLCamera
{
public:
	typedef double FloatType;
	typedef glm::tvec3<FloatType, glm::highp> PositionVec;
	typedef glm::tvec3<FloatType, glm::highp> DirectionVec;

	GLCamera(
		const PositionVec &position = PositionVec(0, 0, 0),
		const FloatType speed = 0.1f,
		const FloatType cursorSensitivity = 0.1f);
	

	void update(GLFWwindow * window);

	inline const glm::mat4 &getViewMatrix();
	inline const DirectionVec &getFront();
	inline const PositionVec getPosition();
protected:
	//bug: front is reversed in direction
	DirectionVec front;
	PositionVec position;
	DirectionVec right;
	DirectionVec up;
	glm::mat4 viewMatrix;
	FloatType speed, cursorSensitivity;

	virtual void updateKeyCallback(GLFWwindow * window);
	virtual void updateCursorCallback(GLFWwindow * window);
};

inline const glm::mat4 &GLCamera::getViewMatrix()
{
	return viewMatrix;
}

inline const GLCamera::DirectionVec & GLCamera::getFront()
{
	return front;
}

inline const GLCamera::PositionVec GLCamera::getPosition()
{
	return position;
}

inline void GLCamera::update(GLFWwindow * window)
{
	updateKeyCallback(window);
	updateCursorCallback(window);
	viewMatrix = glm::lookAt(position, position - front, up);
}
