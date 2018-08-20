#pragma once

#define GLEW_BUILD GLEW_STATIC

#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include "GLTexture.h"
#include "GLShader.h"

#include "Player.h"
#include "World.h"
#include "GLLighting.h"
#include "GLFrameBuffer.hpp"

void initEnv();

void updateEnv();

void updateWindowKeyCallback();
