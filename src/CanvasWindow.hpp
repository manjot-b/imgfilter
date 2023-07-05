#pragma once

#include <glad/glad.h>

#include "IWindow.hpp"

class CanvasWindow : public IWindow
{
	private:
		GLuint m_activeTextureID;
		int m_canvasWidth;
		int m_canvasHeight;

	public:
		CanvasWindow();
		virtual void Render() override;	
		void SetImage(GLuint textureID);
};
