#pragma once

#include <imgui.h>
#include <memory>

#include "Image.hpp"
#include "IWindow.hpp"

class CanvasWindow : public IWindow
{
	public:
		CanvasWindow();
		virtual void Render() override;	
		void SetImage(std::shared_ptr<const Image> image, const std::string& title);

	private:
		std::shared_ptr<const Image> m_activeImage;
		std::string m_title;

		const int m_maxWidth;
		const int m_maxHeight;
		ImVec2 m_windowPadding;

		ImVec2 calcCanvasSize();
};
