#pragma once

#include <memory>
#include <imgui.h>

#include "Image.hpp"
#include "IWindow.hpp"

class CanvasWindow : public IWindow
{
	public:
		CanvasWindow();
		virtual void Render() override;	
		void SetImage(std::shared_ptr<const Image> image);

	private:
		std::shared_ptr<const Image> m_activeImage;
		const int m_maxWidth;
		const int m_maxHeight;
		ImVec2 m_windowPadding;

		ImVec2 calcCanvasSize();
};
