#pragma once

#include <mutex>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <GLFW\glfw3.h>

class GLCamera
{
public:
	typedef glm::vec3 PositionVec;
	typedef glm::vec3 DirectionVec;

	GLCamera(
		const PositionVec &position = PositionVec(0, 0, 0),
		const float speed = 1.0f,
		const float cursorSensitivity = 0.1f);
	

	void update(GLFWwindow * window);

	inline const glm::mat4 &getViewMatrix();
	inline const glm::vec3 &getFront();
	inline const glm::vec3 getPosition();
protected:
	//bug: front is reversed in direction
	DirectionVec front;
	PositionVec position;
	DirectionVec right;
	DirectionVec up;
	glm::mat4 viewMatrix;
	float speed, cursorSensitivity;

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
