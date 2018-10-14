#include "Texture.h"

std::array<GLTexture, static_cast<size_t>(Block::Type::COUNT)> textures;

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

	//must initialize before use
	for (auto &i : textures)
		i = GLTexture();

	textures[static_cast<size_t>(Block::Type::AIR)];
	textures[static_cast<size_t>(Block::Type::DIRT)].load(path + "dirt.png");
	textures[static_cast<size_t>(Block::Type::COBBLESTONE)].load(path + "cobblestone.bmp");
	textures[static_cast<size_t>(Block::Type::GRASS)].load(path + "grass.png");
	textures[static_cast<size_t>(Block::Type::BEDROCK)].load(path + "bedrock.bmp");
	textures[static_cast<size_t>(Block::Type::SAND)].load(path + "sand.png");
	textures[static_cast<size_t>(Block::Type::WATER)].load(path + "water.png");
	textures[static_cast<size_t>(Block::Type::LEAVES)].load(path + "leaves.png");
	textures[static_cast<size_t>(Block::Type::BARK)].load(path + "bark.png");
}

GLTexture Texture::get(Block::Type type)
{
	return textures[static_cast<size_t>(type)];
}

glm::vec2 Texture::getTextureAtlasCoords(const unsigned short & subTextureID, const glm::vec2 &subTexCoords)
{
	return GLTexture::getTextureAtlasCoords(8, 8, subTextureID, subTexCoords);
}