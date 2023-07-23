#pragma once

#include "IWindow.hpp"

class FilterParamsWindow : public IWindow
{
	public:
		FilterParamsWindow();
		virtual void Render(FilterParams& filterParams) override;	
};
