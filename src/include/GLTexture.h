#pragma once
#include <iostream>
#include <string>
#include <vector>

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <SOIL/SOIL.h>


class GLTexture
{
public:
	GLTexture();

	bool load(const std::string &path);

	bool loadMipMap(const std::vector<std::string>& paths);

	void use();

	glm::vec2 static getTextureAtlasCoords(const unsigned short & xNum, const unsigned short & yNum, const unsigned short & subTextureID, const glm::vec2 &subTexCoords);
private:
	GLuint id;
};