#pragma once

#include "IWindow.hpp"

#include <vector>

#include "ProfilerInfo.hpp"

class ProfilerWindow : public IWindow
{
	public:
		struct NamedProfilerInfo
		{
			NamedProfilerInfo(const std::string& name, const ProfilerInfo& profilerInfo) :
				m_name(name), m_profilerInfo(profilerInfo) {}
			std::string m_name;
			ProfilerInfo m_profilerInfo;
		};
		ProfilerWindow();
		virtual void Render(FilterParams& filterParams) override;	

		void AddImageProfiler(const std::string& name, const ProfilerInfo& profilerInfo);
		void ClearImageProfilers();

	private:
		std::vector<NamedProfilerInfo> m_profilerInfos;
};
