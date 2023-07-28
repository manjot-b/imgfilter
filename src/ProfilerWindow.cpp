#include "ProfilerWindow.hpp"

#include <imgui.h>

#include "ProfilerInfo.hpp"

ProfilerWindow::ProfilerWindow() : IWindow("Profiler") {}

void ProfilerWindow::Render(FilterParams& filterParams)
{

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None;
	ImGui::Begin(m_windowName.c_str(), nullptr, windowFlags);

	int headersCount = static_cast<int>(ProfilerTimes::LAST) + 1;
	if (ImGui::BeginTable("Profiler Table", headersCount))
	{
		ImGui::TableSetupColumn("Image");
		for (uint i = 0; i < static_cast<uint>(ProfilerTimes::LAST); ++i)
		{
			ImGui::TableSetupColumn(PROFILER_TIMES_TO_STRING.at(static_cast<ProfilerTimes>(i)).c_str());
		}
		ImGui::TableHeadersRow();

		for (const NamedProfilerInfo& namedProfiler : m_profilerInfos)
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();

			ImGui::Text("%s", namedProfiler.m_name.c_str());

			for (uint i = 0; i < static_cast<uint>(ProfilerTimes::LAST); ++i)
			{
				ImGui::TableNextColumn();

				auto profilerInfoItr = namedProfiler.m_profilerInfo.find(static_cast<ProfilerTimes>(i));

				if (profilerInfoItr != namedProfiler.m_profilerInfo.end())
				{
					ImGui::Text("%.3f", profilerInfoItr->second);
				}
				else
				{
					ImGui::Text("--");
				}
			}
		}

		ImGui::EndTable();
	}

	ImGui::End();
}

void ProfilerWindow::AddImageProfiler(const std::string& name, const ProfilerInfo& profilerInfo)
{
	m_profilerInfos.emplace_back(name, profilerInfo);
}

void ProfilerWindow::ClearImageProfilers() { m_profilerInfos.clear(); }
