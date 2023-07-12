#pragma once

#include <memory>
#include <vector>

#include "Image.hpp"

namespace ImageFilter
{
	std::vector<std::shared_ptr<Image>> GetFilteredImages(std::shared_ptr<Image> originalImage);
}


