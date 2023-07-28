#include "PreviewWindow.hpp"

PreviewWindow::PreviewWindow() :
	IWindow("Preview"),
	m_maxThumbnailWidth(96),
	m_maxThumbnailHeight(96)
{}

void PreviewWindow::Render(FilterParams& filterParams)
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
		for (uint i = 0; i < m_thumbnails.size(); ++i)
		{
			const auto& thumbnail = m_thumbnails[i];
			ImGui::TableNextColumn();

			ImVec2 thumbnailSize = calcThumbnailSize(thumbnail);
			bool buttonPressed = ImGui::ImageButton(
					thumbnail.m_name.c_str(),
					reinterpret_cast<void*>(thumbnail.m_image->GetTextureID()),
					thumbnailSize);

			if (buttonPressed == true)
			{

				notifyThumbnailSelect(thumbnail, i);
			}

			ImGui::Text("%s", thumbnail.m_name.c_str());
		}
		ImGui::EndTable();
	}
	
	ImGui::PopStyleVar();
	ImGui::End();
}

ImVec2 PreviewWindow::calcThumbnailSize(const Thumbnail& thumbnail)
{
	std::shared_ptr<const Image> image = thumbnail.m_image;

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


void PreviewWindow::AddThumbnail(const std::string& name, std::shared_ptr<const Image> image)
{
	m_thumbnails.emplace_back(name, image);
}

void PreviewWindow::ClearThumbnails()
{
	m_thumbnails.clear();
}

size_t PreviewWindow::CountThumbnails()
{
	return m_thumbnails.size();
}

const Thumbnail& PreviewWindow::ThumbnailAt(size_t pos)
{
	return m_thumbnails[pos];
}

void PreviewWindow::AddThumbnailSelectCallback(ThumbnailSelectFunction callback)
{
	m_thumbnailSelectCallbacks.push_back(callback);
}

void PreviewWindow::notifyThumbnailSelect(const Thumbnail& thumbnail, uint index)
{
	for (const auto& callback : m_thumbnailSelectCallbacks)
	{
		callback(thumbnail, index);
	}
}
