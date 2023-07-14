#include <hip/hip_runtime.h>
#include <memory>
#include <vector>

#include "Image.hpp"

namespace ImageFilter {
std::vector<uchar> computeGrayScale(const uchar* d_inImage, const std::shared_ptr<const Image> originalImage, dim3 blockDim, dim3 gridDim);

std::vector<uchar> computeSepia(const uchar* d_inImage, const std::shared_ptr<const Image> originalImage, dim3 blockDim, dim3 gridDim);
}
