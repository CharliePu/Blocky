#include <Image.h>

Image::Image(const std::string & path, const int & forceChannel)
{
	data = std::make_shared<unsigned char*>(SOIL_load_image(path.c_str(), &width, &height, &channel, forceChannel));
	success = *data;
}