#include "GLTexture.h"

GLTexture::GLTexture(const std::string &path, const Type &type = GLTexture::Type::DIFFUSE_MAP)
{
	this->type = type;
	//load image
	int width, height, channel;
	unsigned char* image = SOIL_load_image(path.c_str(), &width, &height, &channel, SOIL_LOAD_AUTO);
	if (!image)
	{
		std::cerr << "Texture: Failed to load image" + path << std::endl;
		return;
	}

	//bind texture
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, (channel > 3 ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);


	//free data
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLTexture::GLTexture(std::vector<std::string> paths)
{
	glGenTextures(1, &this->id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->id);
	int width, height;
	unsigned char* image;
	for (GLuint i = 0; i < 6; i++)
	{
		image = SOIL_load_image(paths[i].c_str(), &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		SOIL_free_image_data(image);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

//y value is inversed, still subTexCoords should inversed y value
glm::vec2 GLTexture::getTextureAtlasCoords(const unsigned short & xNum, const unsigned short & yNum, const unsigned short & subTextureID, const glm::vec2 &subTexCoords)
{
	GLfloat xInterval = 1 / (GLfloat)xNum;
	GLfloat yInterval = 1 / (GLfloat)yNum;
	GLushort yPos = subTextureID / xNum;
	GLushort xPos = subTextureID % xNum;
	//return glm::vec2(subTexCoords.x, subTexCoords.y+yInterval);
	return glm::vec2((subTexCoords.x + xPos)*xInterval, (subTexCoords.y + yPos)*yInterval);
}