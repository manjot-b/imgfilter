#pragma once

#include <iostream>
#include <glad/glad.h>

using uchar = unsigned char;

class Image
{
	private:
		const std::string& m_filename;
		uchar* m_imageData;
		int m_width;
		int m_height;
		int m_components;
		GLuint m_textureID;

		void createOpenGLTexture();

	public:
		Image(const std::string& filename);
		Image(const std::string& filename, const uchar* imageData, int width, int height, int components);
		~Image();

		const std::string& GetFilename();
		const uchar* GetData();
		int GetWidth();
		int GetHeight();
		int GetComponents();
		GLuint GetTextureID();
		void PrintInfo();
		void Write();
		void Write(const std::string& filename);
};
