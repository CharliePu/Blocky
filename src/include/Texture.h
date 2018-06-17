#pragma once
#include <string>
#include <vector>

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <SOIL\SOIL.h>
#include <glm\glm.hpp>

#include "Debug.h"


enum textureType
{
	texture_diffuse_map,
	texture_specular_map,
	texture_reflection_map,
	texture_cube_map,
	texture_atlas
};

class Texture
{
public:
	textureType type;
	std::string path;

	//load diffuse map or specular map
	Texture(const std::string &path, const textureType &type);
	//load cube map
	Texture(const std::vector<std::string> paths);
	//load manually
	Texture(const GLuint &id, const textureType &type);
	//do nothing
	Texture() {};


	operator GLuint();
private:
	GLuint id;
};

glm::vec2 getTextureAtlasCoords(const unsigned short & xNum, const unsigned short & yNum, const unsigned short & subTextureID, const glm::vec2 &subTexCoords);