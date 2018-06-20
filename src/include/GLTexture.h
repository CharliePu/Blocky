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
	enum Type
	{
		DIFFUSE_MAP,
		SPECULAR_MAP,
		REFLECTION_MAP,
		CUB_MAP,
		ATLAS
	};

	Type type;
	std::string path;

	//load diffuse map or specular map
	GLTexture(const std::string &path, const Type &type);
	//load cube map
	GLTexture(const std::vector<std::string> paths);
	//do nothing
	GLTexture() = default;

	operator GLuint();

	glm::vec2 static getTextureAtlasCoords(const unsigned short & xNum, const unsigned short & yNum, const unsigned short & subTextureID, const glm::vec2 &subTexCoords);
private:
	GLuint id;
};

inline GLTexture::operator GLuint()
{
	return id;
}
