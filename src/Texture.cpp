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
	textures[Block::Type::DIRT].load(path + "dirt.png");
	textures[Block::Type::COBBLESTONE].load(path + "cobblestone.bmp");
	textures[Block::Type::GRASS].load(path + "grass.bmp");
	textures[Block::Type::BEDROCK].load(path + "bedrock.bmp");
	textures[Block::Type::SAND].load(path + "sand.png");
	textures[Block::Type::WATER].load(path + "water.png");
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