#pragma once

#include <memory>
#include <tuple>
#include <vector>

#include "FilterParams.hpp"
#include "Image.hpp"
#include "ProfilerInfo.hpp"

namespace ImageFilter
{
	using ImageProfile = std::tuple<std::shared_ptr<Image>, ProfilerInfo>;
	std::vector<ImageProfile> GetFilteredImages(
			std::shared_ptr<const Image> originalImage,
			FilterParams& filteredParams);
}


