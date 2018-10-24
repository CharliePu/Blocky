#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include "Image.h"

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>


class GLTexture
{
public:
	GLTexture();

	virtual bool load(const std::vector<std::string> &paths);
	virtual void use();
protected:

	GLuint id;
};

class GLTextureArray: public GLTexture
{
	virtual bool load(const std::vector<std::string> &paths) override;
	virtual void use() override;
	
};