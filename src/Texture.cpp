#include "Texture.h"

std::array<GLTexture, Block::typeNum> Texture::textures;

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
	textures[Block::Type::AIR]			= GLTexture();
	textures[Block::Type::DIRT]			= GLTexture(path + "dirt.bmp", GLTexture::DIFFUSE_MAP);
	textures[Block::Type::COBBLESTONE]	= GLTexture(path + "cobblestone.bmp", GLTexture::DIFFUSE_MAP);
	textures[Block::Type::GRASS]		= GLTexture(path + "grass.bmp", GLTexture::DIFFUSE_MAP);
	textures[Block::Type::BEDROCK]		= GLTexture(path + "bedrock.bmp", GLTexture::DIFFUSE_MAP);
}