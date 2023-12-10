#include <memory>
#include <stdint.h>
#include <vector>

#include "Image.hpp"
#include "math/vec.hpp"
#include "ProfilerInfo.hpp"

namespace ImageFilter { namespace detail {

template <class... Args>
using GeneralKernelComputeFunction = void (*) (
		const uint8_t* inImage,
		uint8_t* outImage,
		uint32_t pixels,
		uint32_t components,
		Args... args
);

template <class... Args>
std::vector<uint8_t> generalKernelCompute(
		ProfilerInfo& profilerInfo,
		const uint8_t* d_inImage,
		std::shared_ptr<const Image> originalImage,
		uVec3 blockDim,
		uVec3 gridDim,
		GeneralKernelComputeFunction<Args...> kernelComputeFunction,
		Args... args
);

std::vector<uint8_t> nonSeparableKernelCompute(
		ProfilerInfo& profilerInfo,
		const uint8_t* d_paddedInImage,
		std::shared_ptr<const Image> originalImage,
		const float* d_filter,
		uint32_t filterDim,
		uVec3 blockDim,
		uVec3 gridDim
);

std::vector<uint8_t> computeGrayScale(
		ProfilerInfo& profilerInfo,
		const uint8_t* d_inImage,
		std::shared_ptr<const Image> originalImage,
		uVec3 blockDim,
		uVec3 gridDim
);

std::vector<uint8_t> computeSepia(
		ProfilerInfo& profilerInfo,
		const uint8_t* d_inImage,
		std::shared_ptr<const Image> originalImage,
		float k,
		uVec3 blockDim,
		uVec3 gridDim
);

std::vector<uint8_t> computeBoxBlur(
		ProfilerInfo& profilerInfo,
		const uint8_t* d_paddedInImage,
		std::shared_ptr<const Image> originalImage,
		uint32_t filterDim,
		uVec3 blockDim,
		uVec3 gridDim
);

std::vector<uint8_t> computeGaussianBlur(
		ProfilerInfo& profilerInfo,
		const uint8_t* d_paddedInImage,
		std::shared_ptr<const Image> originalImage,
		float sigma,
		uint32_t filterDim,
		uVec3 blockDim,
		uVec3 gridDim
);

std::vector<uint8_t> padRepeatEdge(std::shared_ptr<const Image> image, uVec2 filterDim, uint32_t& width, uint32_t& height);

std::vector<float> generateGaussianFilter(float sigma, uint32_t dim);
}} // namespace ImageFilter::detail
