#include "Image.hpp"
#include <cstring>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

Image::Image(const std::string& filename)
	: m_filename(filename), m_imageData(nullptr), m_textureID(0)
{
 	uchar* imageData = stbi_load(filename.c_str(), &m_width, &m_height, &m_components, 0);

	if (imageData != nullptr)
	{
		size_t bytes = m_width * m_height * m_components;
		m_imageData = new uchar[bytes];

		std::memcpy(m_imageData, imageData, bytes);
		stbi_image_free(imageData);

		createOpenGLTexture();
	}
}

Image::Image(const std::string& filename, const uchar* imageData, int width, int height, int components)
	: m_filename(filename),
	m_imageData(nullptr),
	m_width(width),
	m_height(height),
	m_components(components),
	m_textureID(0)
{
	size_t bytes = m_width * m_height * m_components;
	m_imageData = new uchar[bytes];
	std::memcpy(m_imageData, imageData, bytes);

	createOpenGLTexture();
}

Image::~Image()
{
	if (m_imageData != nullptr)
	{
		delete[] m_imageData;
		glDeleteTextures(1, &m_textureID);
	}
}

void Image::createOpenGLTexture()
{
	glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_2D, m_textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // don't align any of the rows
	switch(m_components)
	{
		case 1:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_width, m_height, 0, GL_RED, GL_UNSIGNED_BYTE, m_imageData);
			break;
		case 2:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, m_width, m_height, 0, GL_RG, GL_UNSIGNED_BYTE, m_imageData);
			break;
		case 3:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_imageData);
			break;
		case 4:
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_imageData);
			break;
		default: break;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

const std::string& Image::GetFilename() { return m_filename; }

const uchar* Image::GetData() { return m_imageData; }

int Image::GetWidth() { return m_width; }

int Image::GetHeight() { return m_height; }

int Image::GetComponents() { return m_components; }

GLuint Image::GetTextureID() { return m_textureID; }

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

