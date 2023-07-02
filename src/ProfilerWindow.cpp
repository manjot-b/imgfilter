#include "ProfilerWindow.hpp"

#include <imgui.h>

ProfilerWindow::ProfilerWindow() : IWindow("Profiler") {}

void ProfilerWindow::Render()
{

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None;
	ImGui::Begin(m_windowName.c_str(), nullptr, windowFlags);

	ImGui::Text("some text in profiler");
	ImGui::Button("a button in profiler");

	ImGui::End();
}
