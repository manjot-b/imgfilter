#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using uchar = unsigned char;

uchar* ReadImage(const std::string& filename)
{
	int width, height, components;
	unsigned char* image = stbi_load(filename.c_str(), &width, &height, &components, 0);

	if (image != nullptr)
	{
		std::cout << filename << " info:\n";
	}

	std::cout << "width: " << width << " height: " << height << " color: ";
	switch(components)
	{
		case 1:
			std::cout << "Grey"; break;
		case 2:
			std::cout << "GreyA"; break;
		case 3:
			std::cout << "RGB"; break;
		case 4:
			std::cout << "RGBA"; break;
	}
	std::cout << '\n';

	return image;
}

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <image>\n";
		exit(1);
	}

	std::string imageFilename(argv[1]);
	std::cout << "Reading in " << imageFilename << '\n';

	uchar* image = ReadImage(imageFilename);

	if (image == nullptr)
	{
		std::cerr << "Error: Could not read image " << imageFilename << '\n';	
		exit(1);
	}

	stbi_image_free(image);
}
