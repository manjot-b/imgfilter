#pragma once

#include <memory>
#include <vector>

#include "FilterParams.hpp"
#include "Image.hpp"
#include "ProfilerInfo.hpp"

namespace ImageFilter
{
	struct FilteredImageResult {
		FilteredImageResult(std::shared_ptr<Image> image, ProfilerInfo profilerInfo) :
			m_image(image), m_profilerInfo(profilerInfo) {}
		std::shared_ptr<Image> m_image;
		ProfilerInfo m_profilerInfo;
	};
	std::vector<FilteredImageResult> GetFilteredImages(
			std::shared_ptr<const Image> originalImage,
			FilterParams& filteredParams);
}


