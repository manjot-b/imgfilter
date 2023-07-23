#pragma once

#include <string>

#include "FilterParams.hpp"

class IWindow
{
	public:
		virtual ~IWindow() {}
		/**
		 * The ImGUI render function for a window. This method should call
		 * ImGui::Begin and ImGui::End().
		 */
		virtual void Render(FilterParams& filterParams) = 0;

		const char* GetWindowName() { return m_windowName.c_str(); }

	protected:
		IWindow(const std::string& windowName) :
			m_windowName(windowName) {}
		const std::string m_windowName;
};
