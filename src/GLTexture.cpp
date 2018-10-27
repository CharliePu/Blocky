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

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, *image.data);
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
	int height, width, mipmap;
	for (auto &i : paths)
	{
		images.emplace_back(i);
		if (!images.back().success)
			return false;
	}

	height = images.front().height;
	width = images.front().width;

	glBindTexture(GL_TEXTURE_2D_ARRAY, id);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, width, height, images.size(), 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	for (auto i = 0u; i != images.size(); ++i)
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, *images[i].data);

	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

	
	return true;
}

void GLTextureArray::use()
{
	glBindTexture(GL_TEXTURE_2D_ARRAY, id);
}