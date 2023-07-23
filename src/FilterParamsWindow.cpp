#include "FilterParamsWindow.hpp"

#include <imgui.h>

FilterParamsWindow::FilterParamsWindow() : IWindow("Filter Parameters") {}

void FilterParamsWindow::Render(FilterParams& filterParams)
{
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None;
	ImGui::Begin(m_windowName.c_str(), nullptr, windowFlags);

	ImGui::SliderFloat("Sepia k", &filterParams.m_sepia.k, 0.f, 1.f);

	ImGui::End();
}
