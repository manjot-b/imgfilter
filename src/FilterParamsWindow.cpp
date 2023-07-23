#include "FilterParamsWindow.hpp"

#include <imgui.h>

FilterParamsWindow::FilterParamsWindow() : IWindow("Filter Parameters") {}

void FilterParamsWindow::Render(FilterParams& filterParams)
{
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None;
	ImGui::Begin(m_windowName.c_str(), nullptr, windowFlags);

	ImGui::Text("some text in filter params");
	ImGui::Button("a button in filter params");

	ImGui::End();
}
