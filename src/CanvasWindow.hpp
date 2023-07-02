#pragma once

#include "IWindow.hpp"

class CanvasWindow : public IWindow
{
	public:
		CanvasWindow();
		virtual void Render() override;	
};
