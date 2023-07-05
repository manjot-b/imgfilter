#include "CanvasWindow.hpp"

#include <cstdint>
#include <imgui.h>

CanvasWindow::CanvasWindow() :
	IWindow("Canvas"),
	m_maxWidth(800),
	m_maxHeight(450)
{}

void CanvasWindow::Render()
{

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None;
	ImGui::Begin(m_windowName.c_str(), nullptr, windowFlags);

	ImGui::Text("some text in canvas");
	ImGui::Button("a button in canvas");

	if (m_activeImage != nullptr && m_activeImage->GetTextureID() != 0)
	{
		ImVec2 canvasSize = calcCanvasSize();
		ImGui::Image(reinterpret_cast<void*>(m_activeImage->GetTextureID()), canvasSize);
	}

	ImGui::End();
}

void CanvasWindow::SetImage(std::shared_ptr<const Image> image) { m_activeImage = image; }

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
