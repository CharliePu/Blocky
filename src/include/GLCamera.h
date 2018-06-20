#pragma once
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <GLFW\glfw3.h>

class GLCamera
{
public:
	GLCamera(
		const glm::vec3 &position = glm::vec3(0, 0, 0),
		const float cursorSensitivity = 0.1f,
		const float speed = 20.0f);
	
	glm::mat4 getViewMatrix();

	void update(GLFWwindow * window);

	glm::vec3 front;
	glm::vec3 position;
private:
	glm::vec3 right;
	glm::vec3 up;
	float speed, cursorSensitivity;

	void updateKeyCallback(GLFWwindow * window);
	void updateCursorCallback(GLFWwindow * window);
};

inline glm::mat4 GLCamera::getViewMatrix()
{
	return glm::lookAt(position, position - front, up);
}

inline void GLCamera::update(GLFWwindow * window)
{
	updateKeyCallback(window);
	updateCursorCallback(window);
}
