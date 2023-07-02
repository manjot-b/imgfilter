#pragma once

#include <string>

class IWindow
{
	public:
		/**
		 * The ImGUI render function for a window. This method should call
		 * ImGui::Begin and ImGui::End().
		 */
		virtual void Render() = 0;

		const char* GetWindowName() { return m_windowName.c_str(); }

	protected:
		IWindow(const std::string& windowName) :
			m_windowName(windowName) {}
		const std::string m_windowName;
};
