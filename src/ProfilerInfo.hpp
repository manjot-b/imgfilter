# pragma once

#include <map>
#include <string>

/**
 * The enum that holds all possible elapsed times that can be recored.
 *
 * @note Not all times makes sense for all kernel computations. Only the relevant times should be recored.
 */
enum class ProfilerTimes
{
	MemCopyHostToDevice,
	MemCopyDeviceToHost,
	KernelCompute,
	LAST // Not an actual value. Used to iterate through enum.
};

const extern std::map<ProfilerTimes, std::string> PROFILER_TIMES_TO_STRING;

/**
 * A map to hold the the time it takes for copying buffers to and from the device and the kernel compute duration.
 *
 * @note All elapsed times are recorded in milliseconds.
 */
using ProfilerInfo = std::map<ProfilerTimes, float>;
