#include "FilterParamsWindow.hpp"
#include "FilterParams.hpp"

#include <imgui.h>

FilterParamsWindow::FilterParamsWindow() : IWindow("Filter Parameters") {}

static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void FilterParamsWindow::Render(FilterParams& filterParams)
{
	FilterParams::Sepia& sepia = filterParams.m_sepia;
	FilterParams::BoxBlur& boxBlur = filterParams.m_boxBlur;
	FilterParams::GausBlur& gausBlur = filterParams.m_gausBlur;

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None;
	ImGuiSliderFlags sliderFlags = ImGuiSliderFlags_AlwaysClamp;
	ImGuiSliderFlags sliderLogFlags = sliderFlags | ImGuiSliderFlags_Logarithmic;

	ImGui::Begin(m_windowName.c_str(), nullptr, windowFlags);

	ImGui::Text("Sepia");
	ImGui::SliderFloat("##sepia_k", &sepia.k, 0.f, 1.f, "k = %.2f", sliderFlags);
	ImGui::SameLine(); HelpMarker("k=0 is no sepia filter. k=1 is max sepia intensity.");

	ImGui::Text("Box Blur");
	ImGui::SliderInt("##boxblur_dim", &boxBlur.filterDim, 1, 255, "dim = %d", sliderLogFlags);
	if (boxBlur.filterDim % 2 == 0) { ++boxBlur.filterDim; }

	ImGui::Text("Gaussian Blur");
	ImGui::SliderInt("##gausblur_dim", &gausBlur.filterDim, 1, 255, "dim = %d", sliderLogFlags);
	if (gausBlur.filterDim % 2 == 0) { ++gausBlur.filterDim; }
	ImGui::SameLine(); HelpMarker("The dim should be ceil(6*sigma)");
	ImGui::SliderFloat("##gausblur_sigma", &gausBlur.sigma, .05f, 50.f, "sigma = %.2f", sliderFlags);

	ImGui::End();
}
