#include "PreviewWindow.hpp"

#include <imgui.h>

PreviewWindow::PreviewWindow() : IWindow("Preview") {}

void PreviewWindow::Render()
{

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None;
	ImGui::Begin(m_windowName.c_str(), nullptr, windowFlags);

	ImGui::Text("some text in preview");
	ImGui::Button("a button in preview");

	ImGui::End();
}
