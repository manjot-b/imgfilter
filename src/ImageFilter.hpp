#pragma once

#include <memory>
#include <vector>

#include "FilterParams.hpp"
#include "Image.hpp"
#include "ProfilerInfo.hpp"

namespace ImageFilter
{
	enum class Filter
	{
		None,
		Grayscale,
		Sepia,
		Padding, // Technically not a filter.
		BoxBlur,
		GaussianBlur
	};

	struct FilteredImageResult {
		FilteredImageResult(Filter filter, std::shared_ptr<Image> image, ProfilerInfo profilerInfo) :
			m_filter(filter), m_image(image), m_profilerInfo(profilerInfo) {}
		Filter m_filter;
		std::shared_ptr<Image> m_image;
		ProfilerInfo m_profilerInfo;
	};
	std::vector<FilteredImageResult> GetFilteredImages(
			std::shared_ptr<const Image> originalImage,
			FilterParams& filteredParams);
}


