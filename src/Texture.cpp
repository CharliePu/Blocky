#include "Texture.h"

std::array<GLTexture, Block::typeNum> textures;

void Texture::init(const std::string &path)
{
	if (path.empty())
	{
		std::cerr << "ERROR: Texture::init() received empty path" << std::endl;
	}

	static bool isInit = false;

	if (isInit)
	{
		std::cerr << "ERROR: Texture::init() has been called more than once" << std::endl;
		return;
	}
	for (auto &i : textures)
		i = GLTexture();

	textures[Block::Type::AIR];
	textures[Block::Type::DIRT].load(path + "dirt.bmp");
	textures[Block::Type::COBBLESTONE].load(path + "cobblestone.bmp");
	textures[Block::Type::GRASS].load(path + "grass.bmp");
	textures[Block::Type::BEDROCK].loadMipMap({ path + "atlas.png", 
		path + "atlas-2x.png", 
		path + "atlas-4x.png", 
		path + "atlas-8x.png", 
		path + "atlas-16x.png",
		path + "atlas-32x.png",
		path + "atlas-64x.png" });
}

GLTexture Texture::get(Block::Type type)
{
	return textures[static_cast<size_t>(type)];
}

GLTexture Texture::get(decltype(Block::typeNum) type)
{
	return textures[static_cast<decltype(Block::typeNum)>(type)];
}

glm::vec2 Texture::getTextureAtlasCoords(const unsigned short & subTextureID, const glm::vec2 &subTexCoords)
{
	return GLTexture::getTextureAtlasCoords(8, 8, subTextureID, subTexCoords);
}