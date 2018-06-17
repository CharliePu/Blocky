#pragma once
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <GLFW\glfw3.h>

#include "Debug.h"

void initCamera(const glm::vec3 &position);

void setCameraFront(const glm::vec3 direction);

glm::vec3 getCameraFront();

glm::mat4 getCameraViewMatrix();

glm::vec3 getCameraPosition();

void updateCameraKeyCallback();

void updateCameraCursorCallback();

extern GLFWwindow* window;