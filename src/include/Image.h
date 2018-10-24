#include <SOIL/SOIL.h>
#include <string>

struct Image
{
	Image(const std::string & path, const int & forceChannel = SOIL_LOAD_AUTO);
	~Image();
	int height, width, channel;
	const unsigned char * data;
	bool success;
};