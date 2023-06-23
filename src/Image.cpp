#include "Image.hpp"
#include <cstring>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

Image::Image(const std::string& filename)
	: m_filename(filename), m_imageData(nullptr)
{
 	uchar* imageData = stbi_load(filename.c_str(), &m_width, &m_height, &m_components, 0);

	if (imageData != nullptr)
	{
		size_t bytes = m_width * m_height * m_components;
		m_imageData = new uchar[bytes];

		std::memcpy(m_imageData, imageData, bytes);
		stbi_image_free(imageData);
	}
}

Image::Image(const std::string& filename, const uchar* imageData, int width, int height, int components)
	: m_filename(filename), m_width(width), m_height(height), m_components(components)
{
	size_t bytes = m_width * m_height * m_components;
	std::memcpy(m_imageData, imageData, bytes);
}

Image::~Image()
{
	if (m_imageData != nullptr) { delete[] m_imageData; }
}

const std::string& Image::GetFilename() { return m_filename; }

const uchar* Image::GetData() { return m_imageData; }

int Image::GetWidth() { return m_width; }

int Image::GetHeight() { return m_height; }

int Image::GetComponents() { return m_components; }

void Image::PrintInfo()
{
	std::cout << "width: " << m_width << " height: " << m_height << " color: ";
	switch(m_components)
	{
		case 1:
			std::cout << "Grey"; break;
		case 2:
			std::cout << "GreyA"; break;
		case 3:
			std::cout << "RGB"; break;
		case 4:
			std::cout << "RGBA"; break;
		default:
			std::cout << "Unknown color for " << m_components << " components"; break;
	}
	std::cout << '\n';
}

void Image::Write()
{
	Write(m_filename);
}

void Image::Write(const std::string& filename)
{
	//TODO: Implement
}

