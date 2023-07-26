#include "ProfilerInfo.hpp"

const std::map<ProfilerTimes, std::string> PROFILER_TIMES_TO_STRING = {
	{ ProfilerTimes::MemCopyHostToDevice, "MemCopyHostToDevice" },
	{ ProfilerTimes::MemCopyDeviceToHost, "MemCopyDeviceToHost" },
	{ ProfilerTimes::KernelCompute, "KernelCompute" }
};
