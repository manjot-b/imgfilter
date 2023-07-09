#include "CanvasWindow.hpp"

#include <cstdint>
#include <imgui.h>

CanvasWindow::CanvasWindow() :
	IWindow("Canvas"),
	m_maxWidth(800),
	m_maxHeight(450),
	m_windowPadding(0, 0)
{}

void CanvasWindow::Render()
{
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None;

	ImGuiStyle& style = ImGui::GetStyle();
	ImVec2 oldPadding = style.WindowPadding;

	// Padding needs to be set before ImGui::Begin(). Since the padding
	// is applied before it is calculated it will be lagging behind by 1 frame.
	// This is not a big deal because we are not animating anything.
	style.WindowPadding = m_windowPadding;
	ImGui::Begin(m_windowName.c_str(), nullptr, windowFlags);

	if (m_activeImage != nullptr && m_activeImage->GetTextureID() != 0)
	{
		ImGui::Text("%s", m_title.c_str());

		ImVec2 canvasSize = calcCanvasSize();
		ImVec2 windowSize = ImGui::GetWindowSize();

		// Reduce padding in the y-direction by a scalar to prevent scroll bars 
		m_windowPadding.x = (windowSize.x - canvasSize.x) / 2.f;
		m_windowPadding.y = (windowSize.y - canvasSize.y) / 2.f * .75f;

		ImGui::Image(reinterpret_cast<void*>(m_activeImage->GetTextureID()), canvasSize);
	}

	ImGui::End();
	style.WindowPadding = oldPadding;
}

void CanvasWindow::SetImage(std::shared_ptr<const Image> image, const std::string& title)
{
	m_activeImage = image;
	m_title = title;
}

ImVec2 CanvasWindow::calcCanvasSize()
{
	float canvasRatio = (float)m_maxWidth / m_maxHeight;	
	float imageRatio = (float)m_activeImage->GetWidth() / m_activeImage->GetHeight();

	ImVec2 size(0, 0);
	float scale = 1.f;

	if (imageRatio > canvasRatio)
	{
		scale = (float)m_maxWidth / m_activeImage->GetWidth();
	}
	else
	{
		scale = (float)m_maxHeight / m_activeImage->GetHeight();
	}

	size.x = m_activeImage->GetWidth() * scale;
	size.y = m_activeImage->GetHeight() * scale;

	return size;
}
