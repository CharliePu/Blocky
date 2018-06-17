#include "Camera.h"
glm::vec3 cameraPos;
glm::vec3 cameraFront;
glm::vec3 cameraRight;
glm::vec3 cameraUp;

float cameraSpeed, cursorSensitivity;

void initCamera(const glm::vec3 &position)
{
	sendError("init camera");
	cameraPos = position;
	cameraFront = glm::vec3(0.0f, 0.0f,1.0f);
	cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	cursorSensitivity = 0.1f;
	cameraSpeed = 20.0f;
}

void setCameraFront(const glm::vec3 direction)
{
	sendError("set camera front");
	cameraFront = direction;
}

glm::vec3 getCameraFront()
{
	return -cameraFront;
}

glm::mat4 getCameraViewMatrix()
{
	return glm::lookAt(cameraPos, cameraPos - cameraFront, cameraUp);
}

glm::vec3 getCameraPosition()
{
	return cameraPos;
}

void updateCameraKeyCallback()
{
	static float lastTime, currentTime;
	float deltaTime;
	lastTime = currentTime;
	currentTime = (float)glfwGetTime();
	deltaTime = currentTime - lastTime;

	if (glfwGetKey(window, GLFW_KEY_W))
		cameraPos -= cameraSpeed*cameraFront*deltaTime;
	if (glfwGetKey(window, GLFW_KEY_S))
		cameraPos += cameraSpeed*cameraFront*cameraFront*deltaTime;
	if (glfwGetKey(window, GLFW_KEY_A))
		cameraPos += cameraSpeed*cameraRight*deltaTime;
	if (glfwGetKey(window, GLFW_KEY_D))
		cameraPos -= cameraSpeed*cameraRight*deltaTime;
	if (glfwGetKey(window, GLFW_KEY_SPACE))
		cameraPos += cameraSpeed*cameraUp*deltaTime;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
		cameraPos -= cameraSpeed*cameraUp*deltaTime;
}

void updateCameraCursorCallback()
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
	

	cameraFront = glm::normalize(-glm::vec3(sin(glm::radians(yaw)) * cos(glm::radians(pitch)), sin(glm::radians(pitch)), -cos(glm::radians(yaw)) * cos(glm::radians(pitch))));
	cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));
}