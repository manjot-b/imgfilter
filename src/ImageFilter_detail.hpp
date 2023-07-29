#include <hip/hip_runtime.h>
#include <memory>
#include <vector>

#include "Image.hpp"
#include "ProfilerInfo.hpp"

namespace ImageFilter { namespace detail {

template <class... Args>
using GeneralKernelComputeFunction = void (*) (
		const uchar* inImage,
		uchar* outImage,
		uint pixels,
		uint components,
		Args... args
);

template <class... Args>
std::vector<uchar> generalKernelCompute(
		ProfilerInfo& profilerInfo,
		const uchar* d_inImage,
		std::shared_ptr<const Image> originalImage,
		dim3 blockDim,
		dim3 gridDim,
		GeneralKernelComputeFunction<Args...> kernelComputeFunction,
		Args... args
);

std::vector<uchar> computeGrayScale(
		ProfilerInfo& profilerInfo,
		const uchar* d_inImage,
		std::shared_ptr<const Image> originalImage,
		dim3 blockDim,
		dim3 gridDim
);

std::vector<uchar> computeSepia(
		ProfilerInfo& profilerInfo,
		const uchar* d_inImage,
		std::shared_ptr<const Image> originalImage,
		float k,
		dim3 blockDim,
		dim3 gridDim
);

std::vector<uchar> computeBoxBlur(
		const uchar* d_paddedInImage,
		std::shared_ptr<const Image> originalImage,
		uint filterDim,
		dim3 blockDim,
		dim3 gridDim,
		ProfilerInfo& profilerInfo
);

std::vector<uchar> computeGaussianBlur(
		const uchar* d_paddedInImage,
		std::shared_ptr<const Image> originalImage,
		float sigma,
		uint filterDim,
		dim3 blockDim,
		dim3 gridDim,
		ProfilerInfo& profilerInfo
);

std::vector<uchar> padRepeatEdge(std::shared_ptr<const Image> image, uint2 filterDim, uint& width, uint& height);

std::vector<float> generateGaussianFilter(float sigma, uint dim);
}} // namespace ImageFilter::detail
