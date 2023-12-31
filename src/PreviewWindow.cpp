#include "PreviewWindow.hpp"

PreviewWindow::PreviewWindow() :
	IWindow("Preview"),
	m_maxThumbnailWidth(96),
	m_maxThumbnailHeight(96),
	m_scale(1.f)
{}

void PreviewWindow::Render(FilterParams& filterParams)
{
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None;
	ImGui::Begin(m_windowName.c_str(), nullptr, windowFlags);

	ImVec2 cellPadding(10 * m_scale, 10 * m_scale);
	int columnWidth = m_maxThumbnailWidth + cellPadding.x;
	int columns = ImGui::GetContentRegionAvail().x / columnWidth;
	if (columns <= 0) { columns = 1; }

	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cellPadding);
	if (ImGui::BeginTable("Thumbnails", columns))
	{
		for (const auto& filterThumbnail : m_thumbnails)
		{
			ImageFilter::Filter filter = filterThumbnail.first;
			const Thumbnail& thumbnail = filterThumbnail.second;
			ImGui::TableNextColumn();

			ImVec2 thumbnailSize = calcThumbnailSize(thumbnail);
			bool buttonPressed = ImGui::ImageButton(
					thumbnail.m_name.c_str(),
					reinterpret_cast<void*>(thumbnail.m_image->GetTextureID()),
					thumbnailSize);

			if (buttonPressed == true)
			{

				notifyThumbnailSelect(thumbnail, filter);
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


void PreviewWindow::ReplaceThumbnail(ImageFilter::Filter filter, const std::string& name, std::shared_ptr<const Image> image)
{
	m_thumbnails[filter] = Thumbnail(name, image);
}

void PreviewWindow::ClearThumbnails()
{
	m_thumbnails.clear();
}

size_t PreviewWindow::CountThumbnails()
{
	return m_thumbnails.size();
}

const PreviewWindow::Thumbnail& PreviewWindow::ThumbnailAt(ImageFilter::Filter filter)
{
	return m_thumbnails.at(filter);
}

void PreviewWindow::AddThumbnailSelectCallback(ThumbnailSelectFunction callback)
{
	m_thumbnailSelectCallbacks.push_back(callback);
}

void PreviewWindow::notifyThumbnailSelect(const Thumbnail& thumbnail, ImageFilter::Filter filter)
{
	for (const auto& callback : m_thumbnailSelectCallbacks)
	{
		callback(thumbnail, filter);
	}
}

void PreviewWindow::OnWindowScale(float scale) { m_scale = scale; };
