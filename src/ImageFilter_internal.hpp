#include <hip/hip_runtime.h>
#include <memory>
#include <vector>

#include "Image.hpp"

namespace ImageFilter { namespace internal {
std::vector<uchar> computeGrayScale(const uchar* d_inImage, std::shared_ptr<const Image> originalImage, dim3 blockDim, dim3 gridDim);

std::vector<uchar> computeSepia(const uchar* d_inImage, std::shared_ptr<const Image> originalImage, dim3 blockDim, dim3 gridDim);

std::vector<uchar> computeBoxBlur(
		const uchar* d_paddedInImage,
		std::shared_ptr<const Image> originalImage,
		const float* filter,
		uint2 filterDim,
		dim3 blockDim,
		dim3 gridDim);

std::vector<uchar> padRepeatEdge(std::shared_ptr<const Image> image, uint2 filterDim, uint& width, uint& height);

std::vector<float> generateGaussianFilter(float sigma, uint dim);
}} // namespace ImageFilter::internal
