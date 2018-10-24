#include "GLTexture.h"

GLTexture::GLTexture()
{
	glGenTextures(1, &this->id);
}

bool GLTexture::load(const std::vector<std::string> &path)
{
	Image image(path.front());

	if (!image.success)
	{
		return false;
	}

	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data);
	glGenerateMipmap(GL_TEXTURE_2D);

	return true;
}

void GLTexture::use()
{
	glBindTexture(GL_TEXTURE_2D, id);
}

bool GLTextureArray::load(const std::vector<std::string>& paths)
{
	std::vector<Image> images;
	int height, width;
	for (auto &i : paths)
	{
		images.push_back(Image(i));
		if (!images.front().success)
			return false;
	}

	height = images.front().height;
	width = images.front().width;

	glBindTexture(GL_TEXTURE_2D_ARRAY, id);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, width, height, images.size());
	for (auto i = 0u; i != images.size(); ++i)
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, images.size(), GL_RGBA, GL_UNSIGNED_BYTE, images[i].data);
	
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	
	return true;
}

void GLTextureArray::use()
{
	glBindTexture(GL_TEXTURE_2D_ARRAY, id);
}

//bool GLTexture::loadMultipleAsTextureArray(const std::vector<std::string>& paths)
//{
//	std::vector<int> widths(paths.size()), heights(paths.size()), channels(paths.size());
//	std::vector<unsigned char*> images(paths.size());
//	for (size_t i = 0; i != paths.size(); ++i)
//	{
//		images[i] = SOIL_load_image(paths[i].c_str(), &widths[i], &heights[i], &channels[i], SOIL_LOAD_AUTO);
//		if (!images[i])
//		{
//			return false;
//		}
//	}
//	glBindTexture(GL_TEXTURE_2D_ARRAY, id);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, paths.size() - 1);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//
//	for (size_t i = 0; i != paths.size(); ++i)
//	{
//		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, widths[i], heights[i], i,  0, (channels[i] > 3 ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, images[i]);
//	}
//	glBindTexture(GL_TEXTURE_2D, NULL);
//	return true;
//}
//
//bool GLTexture::loadMipMap(const std::vector<std::string>& paths)
//{
//	std::vector<int> widths(paths.size()), heights(paths.size()), channels(paths.size());
//	std::vector<unsigned char*> images(paths.size());
//	for (size_t i = 0; i != paths.size(); ++i)
//	{
//		images[i] = SOIL_load_image(paths[i].c_str(), &widths[i], &heights[i], &channels[i], SOIL_LOAD_AUTO);
//		if (!images[i])
//		{
//			return false;
//		}
//	}
//	glBindTexture(GL_TEXTURE_2D, id);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, paths.size() - 1);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//
//	for (size_t i = 0; i != paths.size(); ++i)
//	{
//		glTexImage2D(GL_TEXTURE_2D, i, GL_RGBA, widths[i], heights[i], 0, (channels[i] > 3 ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, images[i]);
//	}
//	glBindTexture(GL_TEXTURE_2D, NULL);
//	return true;
//}

////y value is inversed, still subTexCoords should inversed y value
//glm::vec2 GLTexture::getTextureAtlasCoords(const unsigned short & xNum, const unsigned short & yNum, const unsigned short & subTextureID, const glm::vec2 &subTexCoords)
//{
//	GLfloat xInterval = 1 / (GLfloat)xNum;
//	GLfloat yInterval = 1 / (GLfloat)yNum;
//	GLushort yPos = subTextureID / xNum;
//	GLushort xPos = subTextureID % xNum;
//	return glm::vec2((subTexCoords.x + xPos)*xInterval, (subTexCoords.y + yPos)*yInterval);
//}
