#include "GLCamera.h"

GLCamera::GLCamera(
	const GLCamera::PositionVec &position,
	const float speed,
	const float cursorSensitivity) :
	front(0, 0, 1), position(position), up(0, 1, 0),
	speed(speed), cursorSensitivity(cursorSensitivity)
{}


void GLCamera::updateKeyCallback(GLFWwindow * window)
{
	static float lastTime, currentTime;
	float deltaTime;
	lastTime = currentTime;
	currentTime = (float)glfwGetTime();
	deltaTime = currentTime - lastTime;

	if (glfwGetKey(window, GLFW_KEY_W))
		position -= speed * deltaTime * glm::normalize(front * glm::vec3(1, 0, 1));
	if (glfwGetKey(window, GLFW_KEY_S))
		position += speed * deltaTime * glm::normalize(front * glm::vec3(1, 0, 1));
	if (glfwGetKey(window, GLFW_KEY_A))
		position += speed * deltaTime * glm::normalize(right * glm::vec3(1, 0, 1));
	if (glfwGetKey(window, GLFW_KEY_D))
		position -= speed * deltaTime * glm::normalize(right * glm::vec3(1, 0, 1));
	if (glfwGetKey(window, GLFW_KEY_SPACE))
		position += speed * up * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
		position -= speed * up * deltaTime;
}

void GLCamera::updateCursorCallback(GLFWwindow * window)
{
	//compute cursor offset
	static double lastCursorX, lastCursorY;
	double cursorX, cursorY;
	float cursorXOffset, cursorYOffset;
	glfwGetCursorPos(window, &cursorX, &cursorY);
	cursorXOffset = (float)(cursorX - lastCursorX);
	cursorYOffset = (float)(lastCursorY - cursorY);
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

	static float yaw;
	yaw += cursorXOffset*cursorSensitivity;
	static float pitch;
	pitch += cursorYOffset*cursorSensitivity;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;
	

	front = glm::normalize(-glm::vec3(sin(glm::radians(yaw)) * cos(glm::radians(pitch)), sin(glm::radians(pitch)), -cos(glm::radians(yaw)) * cos(glm::radians(pitch))));
	right = glm::normalize(glm::cross(front, up));
}