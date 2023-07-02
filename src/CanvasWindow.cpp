#include "CanvasWindow.hpp"

#include <imgui.h>

CanvasWindow::CanvasWindow() : IWindow("Canvas") {}

void CanvasWindow::Render()
{

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None;
	ImGui::Begin(m_windowName.c_str(), nullptr, windowFlags);

	ImGui::Text("some text in canvas");
	ImGui::Button("a button in canvas");

	ImGui::End();
}
