#pragma once

#include "IWindow.hpp"

#include <tuple>
#include <vector>

#include "ProfilerInfo.hpp"

class ProfilerWindow : public IWindow
{
	public:
		using NamedProfilerInfo = std::tuple<std::string, ProfilerInfo>;
		ProfilerWindow();
		virtual void Render(FilterParams& filterParams) override;	

		void AddImageProfiler(const std::string& name, const ProfilerInfo& profilerInfo);
		void ClearImageProfilers();

	private:
		std::vector<NamedProfilerInfo> m_profilerInfos;
};
