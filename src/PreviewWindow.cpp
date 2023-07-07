#include "PreviewWindow.hpp"

#include <imgui.h>

PreviewWindow::PreviewWindow() :
	IWindow("Preview"),
	m_thumbnailWidth(128),
	m_thumbnailHeight(128)
{}

void PreviewWindow::Render()
{
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None;
	ImGui::Begin(m_windowName.c_str(), nullptr, windowFlags);

	int thumbnailWidth = 96;
	ImVec2 cellPadding(8, 8);
	int columnWidth = thumbnailWidth + cellPadding.x;
	int columns = ImGui::GetContentRegionAvail().x / columnWidth;
	if (columns <= 0) { columns = 1; }

	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cellPadding);
	if (ImGui::BeginTable("Thumbnails", columns))
	{
		for (const auto& thumbnail : m_thumbnails)
		{
			ImGui::TableNextColumn();
			int thumbnailHeight = ((float)thumbnailWidth / thumbnail.m_image->GetWidth()) * thumbnail.m_image->GetHeight();
			ImGui::Image(reinterpret_cast<void*>(thumbnail.m_image->GetTextureID()), ImVec2(thumbnailWidth, thumbnailHeight));
		}
		ImGui::EndTable();
	}
	
	ImGui::PopStyleVar();
	ImGui::End();
}

std::vector<Thumbnail>& PreviewWindow::GetThumbnails() { return m_thumbnails; }
