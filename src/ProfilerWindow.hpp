#pragma once

#include "IWindow.hpp"

class ProfilerWindow : public IWindow
{
	public:
		ProfilerWindow();
		virtual void Render() override;	
};
