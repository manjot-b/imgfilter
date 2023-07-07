#include "PreviewWindow.hpp"

PreviewWindow::PreviewWindow() :
	IWindow("Preview"),
	m_maxThumbnailWidth(96),
	m_maxThumbnailHeight(96)
{}

void PreviewWindow::Render()
{
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None;
	ImGui::Begin(m_windowName.c_str(), nullptr, windowFlags);

	ImVec2 cellPadding(8, 8);
	int columnWidth = m_maxThumbnailWidth + cellPadding.x;
	int columns = ImGui::GetContentRegionAvail().x / columnWidth;
	if (columns <= 0) { columns = 1; }

	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cellPadding);
	if (ImGui::BeginTable("Thumbnails", columns))
	{
		for (const auto& thumbnail : m_thumbnails)
		{
			ImGui::TableNextColumn();
			ImVec2 thumbnailSize = calcThumbnailSize(thumbnail);
			ImGui::Image(reinterpret_cast<void*>(thumbnail.m_image->GetTextureID()), thumbnailSize);
			ImGui::Text("%s", thumbnail.m_name.c_str());
		}
		ImGui::EndTable();
	}
	
	ImGui::PopStyleVar();
	ImGui::End();
}

std::vector<Thumbnail>& PreviewWindow::GetThumbnails() { return m_thumbnails; }

ImVec2 PreviewWindow::calcThumbnailSize(const Thumbnail& thumbnail)
{
	std::shared_ptr<Image> image = thumbnail.m_image;

	float thumbnailWidth = m_maxThumbnailWidth;
	float thumbnailHeight = (thumbnailWidth / image->GetWidth()) * image->GetHeight();

	if (thumbnailHeight > m_maxThumbnailHeight)
	{
		float scale = m_maxThumbnailHeight / thumbnailHeight;
		thumbnailWidth *= scale;
		thumbnailHeight *= scale;
	}

	return ImVec2(thumbnailWidth, thumbnailHeight);
}
