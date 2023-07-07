#pragma once

#include <memory>
#include <vector>

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
		int m_thumbnailWidth;
		int m_thumbnailHeight;
		std::vector<Thumbnail> m_thumbnails;

	public:
		PreviewWindow();
		virtual void Render() override;	
		std::vector<Thumbnail>& GetThumbnails();
};
