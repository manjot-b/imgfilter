#pragma once

#include <imgui.h>
#include <memory>

#include "Image.hpp"
#include "IWindow.hpp"

class CanvasWindow : public IWindow
{
	public:
		CanvasWindow();
		virtual void Render(FilterParams& filterParams) override;	
		void SetImage(const std::string& title, std::shared_ptr<const Image> image);

	private:
		std::string m_title;
		std::shared_ptr<const Image> m_activeImage;

		const int m_maxWidth;
		const int m_maxHeight;
		ImVec2 m_windowPadding;

		ImVec2 calcCanvasSize();
};
