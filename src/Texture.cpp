#include "Texture.h"

void TexturePack::init(const std::string &path)
{
	if (path.empty())
	{
		std::cerr << "ERROR: TexturePack::init() received empty path" << std::endl;
	}

	static bool isInit = false;

	if (isInit)
	{
		std::cerr << "ERROR: TexturePack::init() has been called more than once" << std::endl;
		return;
	}

	paths[static_cast<size_t>(Block::Type::AIR)] = path + "air.png";
	paths[static_cast<size_t>(Block::Type::DIRT)] = path + "dirt.png";
	paths[static_cast<size_t>(Block::Type::COBBLESTONE)] = path + "cobblestone.png";
	paths[static_cast<size_t>(Block::Type::GRASS)] = path + "grass.png";
	paths[static_cast<size_t>(Block::Type::BEDROCK)] = path + "bedrock.png";
	paths[static_cast<size_t>(Block::Type::SAND)] = path + "sand.png";
	paths[static_cast<size_t>(Block::Type::WATER)] = path + "water.png";
	paths[static_cast<size_t>(Block::Type::LEAVES)] = path + "leaves.png";
	paths[static_cast<size_t>(Block::Type::BARK)] = path + "bark.png";


	if (!textures.load(paths))
	{
		std::cerr << "ERROR: TexturePack::init() failed to load" << std::endl;
		return;
	}
}

void TexturePack::apply()
{
	textures.use();
}
