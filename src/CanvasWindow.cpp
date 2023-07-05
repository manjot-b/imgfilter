#include "CanvasWindow.hpp"

#include <cstdint>
#include <imgui.h>

CanvasWindow::CanvasWindow() :
	IWindow("Canvas"),
	m_canvasWidth(600),
	m_canvasHeight(800)
{}

void CanvasWindow::Render()
{

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None;
	ImGui::Begin(m_windowName.c_str(), nullptr, windowFlags);

	ImGui::Text("some text in canvas");
	ImGui::Button("a button in canvas");

	if (m_activeTextureID != 0)
	{
		ImGui::Image(reinterpret_cast<void*>(m_activeTextureID), ImVec2(m_canvasWidth, m_canvasHeight));
	}

	ImGui::End();
}

void CanvasWindow::SetImage(GLuint textureID) { m_activeTextureID = textureID; }
