#pragma once

#include "IWindow.hpp"

class PreviewWindow : public IWindow
{
	public:
		PreviewWindow();
		virtual void Render() override;	
};
