#pragma once

#include "IWindow.hpp"

class PreviewWindow : public IWindow
{
	private:
		int m_thumbnailWidth;
		int m_thumbnailHeight;

	public:
		PreviewWindow();
		virtual void Render() override;	
};
