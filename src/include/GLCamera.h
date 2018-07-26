#pragma once

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <GLFW\glfw3.h>

class GLCamera
{
public:
	GLCamera(
		const glm::vec3 &position = glm::vec3(0, 50, 0),
		const float cursorSensitivity = 0.1f,
		const float speed = 100.0f);
	
	inline const glm::mat4 &getViewMatrix();

	void update(GLFWwindow * window);

	glm::vec3 front;
	glm::vec3 position;
protected:
	glm::vec3 right;
	glm::vec3 up;
	glm::mat4 viewMatrix;
	float speed, cursorSensitivity;

	virtual void updateKeyCallback(GLFWwindow * window);
	virtual void updateCursorCallback(GLFWwindow * window);
};

inline const glm::mat4 &GLCamera::getViewMatrix()
{
	return viewMatrix;
}

inline void GLCamera::update(GLFWwindow * window)
{
	updateKeyCallback(window);
	updateCursorCallback(window);
	viewMatrix = glm::lookAt(position, position - front, up);
}
