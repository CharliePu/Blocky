#pragma once

#define GLEW_BUILD GLEW_STATIC

#include "Texture.h"
#include "Shader.h"
#include "Camera.h"
#include "Chunk.h"
#include "Debug.h"

void initEnv();

void updateEnv();

void updateWindowKeyCallback();
