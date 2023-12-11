#pragma once

#include "IWindow.hpp"

#include <imgui.h>
#include <functional>
#include <map>
#include <memory>
#include <vector>

#include "Image.hpp"
#include "imagefilter/ImageFilter.hpp"

class PreviewWindow : public IWindow
{
	public:
		struct Thumbnail
		{
			Thumbnail() {}
			Thumbnail(const std::string& name, std::shared_ptr<const Image> image) :
				m_name(name), m_image(image) {}
			std::string m_name;
			std::shared_ptr<const Image> m_image;
		};

		PreviewWindow();
		virtual void Render(FilterParams& filterParams) override;	
		void ReplaceThumbnail(ImageFilter::Filter filter, const std::string& name, std::shared_ptr<const Image> image);
		void ClearThumbnails();
		size_t CountThumbnails();
		const Thumbnail& ThumbnailAt(ImageFilter::Filter filter);

		using ThumbnailSelectFunction = std::function<void(const Thumbnail& thumbnail, ImageFilter::Filter filter)>;
		void AddThumbnailSelectCallback(ThumbnailSelectFunction callback);

		void OnWindowScale(float scale);

	private:
		int m_maxThumbnailWidth;
		int m_maxThumbnailHeight;
		float m_scale;

		std::map<ImageFilter::Filter, Thumbnail> m_thumbnails;
		std::vector<ThumbnailSelectFunction> m_thumbnailSelectCallbacks;

		ImVec2 calcThumbnailSize(const Thumbnail& thumbnail);
		void notifyThumbnailSelect(const Thumbnail& thumbnail, ImageFilter::Filter filter);
};
