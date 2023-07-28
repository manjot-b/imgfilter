#pragma once

#include <imgui.h>
#include <functional>
#include <memory>
#include <vector>

#include "Image.hpp"
#include "IWindow.hpp"

struct Thumbnail
{
	Thumbnail();
	Thumbnail(const std::string& name, std::shared_ptr<const Image> image) :
		m_name(name), m_image(image) {}
	std::string m_name;
	std::shared_ptr<const Image> m_image;
};

class PreviewWindow : public IWindow
{
	public:
		PreviewWindow();
		virtual void Render(FilterParams& filterParams) override;	
		void AddThumbnail(const std::string& name, std::shared_ptr<const Image> image);
		void ClearThumbnails();
		size_t CountThumbnails();
		const Thumbnail& ThumbnailAt(size_t pos);

		using ThumbnailSelectFunction = std::function<void(const Thumbnail& thumbnail, uint index)>;
		void AddThumbnailSelectCallback(ThumbnailSelectFunction callback);

	private:
		int m_maxThumbnailWidth;
		int m_maxThumbnailHeight;
		std::vector<Thumbnail> m_thumbnails;
		std::vector<ThumbnailSelectFunction> m_thumbnailSelectCallbacks;

		ImVec2 calcThumbnailSize(const Thumbnail& thumbnail);
		void notifyThumbnailSelect(const Thumbnail& thumbnail, uint index);
};
