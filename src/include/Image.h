#include <SOIL/SOIL.h>
#include <string>
#include <memory>
#include <iostream>

struct Image
{
	Image(const std::string & path, const int & forceChannel = SOIL_LOAD_AUTO);

	//Image(const Image &image) :
	//	height(image.height),
	//	width(image.width),
	//	data(image.data)
	//{
	//	std::cout << "copy constructor called" << std::endl;
	//}

	//Image(const Image &&image) :
	//	data(std::move(image.data)),
	//	height(std::move(image.height)),
	//	width(std::move(image.width)) {
	//	std::cout << "move constructor called" << std::endl;
	//}

	//Image& operator= (const Image &image)
	//{
	//	return *this = Image(image);
	//}

	//Image& operator= (Image &&image)
	//{
	//	std::swap(image.data, data);
	//	std::swap(image.height, height);
	//	std::swap(image.width, width);
	//}

	int height, width, channel;
	std::shared_ptr<unsigned char*> data;
	bool success;
};