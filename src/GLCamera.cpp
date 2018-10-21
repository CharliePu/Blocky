#include "GLCamera.h"

GLCamera::GLCamera(
	const GLCamera::PositionVec &position,
	const FloatType speed,
	const FloatType cursorSensitivity) :
	front(0, 0, 1), position(position), up(0, 1, 0),
	speed(3), cursorSensitivity(cursorSensitivity)
{}


void GLCamera::updateKeyCallback(GLFWwindow * window)
{
	static FloatType lastTime, currentTime;
	FloatType deltaTime;
	lastTime = currentTime;
	currentTime = glfwGetTime();
	deltaTime = currentTime - lastTime;

	if (glfwGetKey(window, GLFW_KEY_W))
		position -= speed * deltaTime * glm::normalize(front * DirectionVec(1, 0, 1));
	if (glfwGetKey(window, GLFW_KEY_S))
		position += speed * deltaTime * glm::normalize(front * DirectionVec(1, 0, 1));
	if (glfwGetKey(window, GLFW_KEY_A))
		position += speed * deltaTime * glm::normalize(right * DirectionVec(1, 0, 1));
	if (glfwGetKey(window, GLFW_KEY_D))
		position -= speed * deltaTime * glm::normalize(right * DirectionVec(1, 0, 1));
	if (glfwGetKey(window, GLFW_KEY_SPACE))
		position += speed * up * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
		position -= speed * up * deltaTime;
}

void GLCamera::updateCursorCallback(GLFWwindow * window)
{
	//compute cursor offset
	static FloatType lastCursorX, lastCursorY;
	FloatType cursorX, cursorY;
	FloatType cursorXOffset, cursorYOffset;
	glfwGetCursorPos(window, &cursorX, &cursorY);
	cursorXOffset = cursorX - lastCursorX;
	cursorYOffset = lastCursorY - cursorY;
	lastCursorX = cursorX;
	lastCursorY = cursorY;

	//prevent view jump
	static bool init;
	if (!init)
	{
		init = true;
		cursorXOffset = 0;
		cursorYOffset = 0;
	}

	static FloatType yaw;
	yaw += cursorXOffset*cursorSensitivity;
	static FloatType pitch;
	pitch += cursorYOffset*cursorSensitivity;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;
	

	front = glm::normalize(-glm::vec3(sin(glm::radians(yaw)) * cos(glm::radians(pitch)), sin(glm::radians(pitch)), -cos(glm::radians(yaw)) * cos(glm::radians(pitch))));
	right = glm::normalize(glm::cross(front, up));
}