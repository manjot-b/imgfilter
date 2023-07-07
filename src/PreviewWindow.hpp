#pragma once

#include <memory>
#include <vector>
#include <imgui.h>

#include "Image.hpp"
#include "IWindow.hpp"

struct Thumbnail
{
	Thumbnail();
	Thumbnail(const std::string& name, std::shared_ptr<Image> image) :
		m_name(name), m_image(image) {}
	std::string m_name;
	std::shared_ptr<Image> m_image;
};

class PreviewWindow : public IWindow
{
	private:
		int m_maxThumbnailWidth;
		int m_maxThumbnailHeight;
		std::vector<Thumbnail> m_thumbnails;

		ImVec2 calcThumbnailSize(const Thumbnail& thumbnail);

	public:
		PreviewWindow();
		virtual void Render() override;	
		std::vector<Thumbnail>& GetThumbnails();
};
