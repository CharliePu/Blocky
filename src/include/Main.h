#pragma once

#define GLEW_BUILD GLEW_STATIC

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include "GLTexture.h"
#include "GLShader.h"

#include "Camera.h"
#include "World.h"
#include "Debug.h"

void initEnv();

void updateEnv();

void updateWindowKeyCallback();
