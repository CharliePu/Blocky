#include <Image.h>

Image::Image(const std::string & path, const int & forceChannel)
{
	data = SOIL_load_image(path.c_str(), &width, &height, &channel, forceChannel);
	success = data;
}

Image::~Image()
{
	delete[] data;
}
