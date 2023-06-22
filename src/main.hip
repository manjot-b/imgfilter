#include <iostream>

#include "Image.hpp"

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <image>\n";
		exit(1);
	}

	std::string imageFilename(argv[1]);
	std::cout << "Reading in " << imageFilename << '\n';

	Image origImage(imageFilename);

	if (origImage.GetData() == nullptr)
	{
		std::cerr << "Error: Could not read image " << origImage.GetFilename() << '\n';	
		exit(1);
	}

	origImage.PrintInfo();
}
