#pragma once

#include <iostream>
#include <glad/glad.h>
#include <vector>

using uchar = unsigned char;

class Image
{
	public:
		Image(const std::string& filename);
		Image(const std::string& filename, std::vector<uchar>&& imageData, int width, int height, int components);
		~Image();

		const std::string& GetFilename() const;
		const uchar* GetData() const;
		int GetWidth() const;
		int GetHeight() const;
		int GetComponents() const;
		GLuint GetTextureID() const;
		void PrintInfo() const;
		void Write() const;
		void Write(const std::string& filename) const;

	private:
		const std::string m_filename;
		std::vector<uchar> m_imageData;
		int m_width;
		int m_height;
		int m_components;
		GLuint m_textureID;

		void createOpenGLTexture();
};
