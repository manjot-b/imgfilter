#pragma once

#include <memory>
#include <vector>

#include "FilterParams.hpp"
#include "Image.hpp"

namespace ImageFilter
{
	std::vector<std::shared_ptr<Image>> GetFilteredImages(
			std::shared_ptr<const Image> originalImage,
			FilterParams& filteredParams);
}


