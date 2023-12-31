#include "imagefilter/ImageFilter.hpp"
#include "imagefilter/ImageFilter_detail.hpp"

#include <hip/hip_runtime.h>
#include <iostream>
#include <memory>
#include <sstream>

#include "FilterParams.hpp"

__device__ float clamp(float val, float min, float max)
{
	if (val < min) { return min; }
	else if (val > max) { return max; }
	return val;
}

__global__ void GrayScaleFilter(
		const uchar* inImage,
		uchar* outImage,
		unsigned int pixels,
		unsigned int components
)
{
	const unsigned int PIXEL_OFFSET = (blockIdx.x * blockDim.x + threadIdx.x) * components;
	const unsigned int TOTAL_BYTES = pixels * components;

	if (PIXEL_OFFSET >= TOTAL_BYTES) { return; }

	uchar3 color = {0, 0, 0};
	color.x = inImage[PIXEL_OFFSET + 0];
	color.y = inImage[PIXEL_OFFSET + 1];
	color.z = inImage[PIXEL_OFFSET + 2];

	const uchar RGBComponents = 3;
	uchar gray = (color.x + color.y + color.z) / RGBComponents;

	for (int i = 0; i < RGBComponents; i++)	// Assume image is RGB or RGBA
	{
		outImage[PIXEL_OFFSET + i] = gray;
	}
}

__global__ void SepiaFilter(
		const uchar* inImage,
		uchar* outImage,
		unsigned int pixels,
		unsigned int components,
		float k
)
{
	const unsigned int PIXEL_OFFSET = (blockIdx.x * blockDim.x + threadIdx.x) * components;
	const unsigned int TOTAL_BYTES = pixels * components;

	if (PIXEL_OFFSET >= TOTAL_BYTES) { return; }

	uchar3 color = {0, 0, 0};
	color.x = inImage[PIXEL_OFFSET + 0];
	color.y = inImage[PIXEL_OFFSET + 1];
	color.z = inImage[PIXEL_OFFSET + 2];

	// Taken from https://yabirgb.com/sepia_filter/
	float3 sepia;
	sepia.x = (0.393f + 0.607f * (1 - k)) * color.x
		+ (0.769f - 0.769f * (1 - k)) * color.y
		+ (0.189f - 0.189f * (1 - k)) * color.z;
	sepia.y = (0.349f - 0.349f * (1 - k)) * color.x
		+ (0.686f + 0.314f * (1 - k)) * color.y
		+ (0.168f - 0.168f * (1 - k)) * color.z;
	sepia.z = (0.272f - 0.349f * (1 - k)) * color.x
		+ (0.534f - 0.534f * (1 - k)) * color.y
		+ (0.131f + 0.869f * (1 - k)) * color.z;

	sepia.x = clamp(sepia.x, 0, 255);
	sepia.y = clamp(sepia.y, 0, 255);
	sepia.z = clamp(sepia.z, 0, 255);

	outImage[PIXEL_OFFSET + 0] = sepia.x;
	outImage[PIXEL_OFFSET + 1] = sepia.y;
	outImage[PIXEL_OFFSET + 2] = sepia.z;
}

/**
 * Performs a convolution on the input image using a non-separable filter.
 * @param paddedInImage The input image but padded with @c filterDim.x / 2 cols added to each side
 *   and @c filterDim.y / 2 rows added to the top and bottom.
 * @param outImage The image to output to.
 * @param originalDim The dimension of @c paddedInImage were it not padded. Same size as @c outImage.
 * @param components The number of components of @c paddedInImage
 * @param filter A row-major normalized filter.
 * @param The dimensions of @c filter.
 */
__global__ void NonSeparableConvolution(
		const uchar* paddedInImage,
		uchar* outImage,
		uint2 originalDim,
		uint components,
		const float* filter,
		uint2 filterDim
)
{
	// NOTE: Bug on Radeon RX 580 gfx803. Cannot set uint2 variable to const.
	const uint filterRadiusX = filterDim.x / 2;
	const uint filterRadiusY = filterDim.y / 2;
	const uint paddedDimX = originalDim.x + filterRadiusX * 2;

	// Shift the pixel over by the padded rows, then the padded columns, then get to the correct row
	// based on the current pixel from the original unpadded image.
	const uint paddedRowShift = paddedDimX * filterRadiusY * components;
	const uint paddedColShift = filterRadiusX * components;
	const uint PAD_SHIFT = paddedRowShift + paddedColShift;

	const uint originalPixel = blockIdx.x * blockDim.x + threadIdx.x;
	const uint ORIGINAL_PIXEL_OFFSET = originalPixel * components;

	const uint rowIdx = originalPixel / originalDim.x;
	const uint colIdx = originalPixel - originalDim.x * rowIdx;
	const uint PADDED_PIXEL_OFFSET = PAD_SHIFT + (rowIdx * paddedDimX + colIdx) * components;
	const uint LAST_PIXEL_OFFSET = PAD_SHIFT + ( (originalDim.y - 1) * paddedDimX + (originalDim.x - 1) ) * components;

	if (PADDED_PIXEL_OFFSET > LAST_PIXEL_OFFSET) { return; }


	float3 color = {0.f, 0.f, 0.f};
	for (int row = 0; row < filterDim.y; ++row)
	{
		// Get to the correct kernel row
		uint kernelPixelRow = PADDED_PIXEL_OFFSET + (row - filterRadiusY) * paddedDimX * components;
		for (int col = 0; col < filterDim.x; ++col)
		{
			// Offset by the kernel column
			uint kernelPixel = kernelPixelRow + (col - filterRadiusX) * components;

			uint filterIdx = filterDim.x * row + col;
			color.x += paddedInImage[kernelPixel + 0] * filter[filterIdx];
			color.y += paddedInImage[kernelPixel + 1] * filter[filterIdx];
			color.z += paddedInImage[kernelPixel + 2] * filter[filterIdx];
		}
	}
	color.x = clamp(color.x, 0, 255);
	color.y = clamp(color.y, 0, 255);
	color.z = clamp(color.z, 0, 255);

	outImage[ORIGINAL_PIXEL_OFFSET + 0] = color.x;
	outImage[ORIGINAL_PIXEL_OFFSET + 1] = color.y;
	outImage[ORIGINAL_PIXEL_OFFSET + 2] = color.z;
}

namespace ImageFilter {
inline void HIPCheck(hipError_t error)
{
	if (error != hipSuccess)
	{
		std::cerr << "An error occurred: \"" << hipGetErrorString(error) << "\"\n";
	}
}

std::vector<FilteredImageResult> GetFilteredImages(
		std::shared_ptr<const Image> originalImage,
		FilterParams& filterParams
)
{
	using namespace detail;

	hipEvent_t memCopyStart, memCopyEnd;
	HIPCheck(hipEventCreate(&memCopyStart));
	HIPCheck(hipEventCreate(&memCopyEnd));

	size_t pixels = originalImage->GetWidth() * originalImage->GetHeight();
	size_t bytes = sizeof(uint8_t) * pixels * originalImage->GetComponents();

	uint8_t* d_inImage = nullptr;
	HIPCheck(hipMalloc(&d_inImage, bytes));

	HIPCheck(hipEventRecord(memCopyStart, nullptr));

	HIPCheck(hipMemcpy(d_inImage, originalImage->GetData(), bytes, hipMemcpyHostToDevice));

	HIPCheck(hipEventRecord(memCopyEnd, nullptr));
	HIPCheck(hipEventSynchronize(memCopyEnd));

	float memCopyElapsed;
	HIPCheck(hipEventElapsedTime(&memCopyElapsed, memCopyStart, memCopyEnd));

	constexpr unsigned int threads = 64;
	uVec3 blockDim = uVec3(threads, 1, 1);
	uVec3 gridDim = uVec3((pixels + threads - 1) / threads, 1, 1);	// ceiling(pixels / threads)

	std::vector<FilteredImageResult> filteredImages;
	std::vector<uint8_t> outImageData;
	ProfilerInfo profilerInfo;

	outImageData = computeGrayScale(profilerInfo, d_inImage, originalImage, blockDim, gridDim);
	profilerInfo[ProfilerTimes::MemCopyHostToDevice] = memCopyElapsed;
	filteredImages.emplace_back(
				Filter::Grayscale,
				std::make_shared<Image>(
					"imgs/gray.png",
					std::move(outImageData),
					originalImage->GetWidth(),
					originalImage->GetHeight(),
					originalImage->GetComponents()
				),
				profilerInfo
	);

	outImageData = computeSepia(profilerInfo, d_inImage, originalImage, filterParams.m_sepia.k, blockDim, gridDim);
	profilerInfo[ProfilerTimes::MemCopyHostToDevice] = memCopyElapsed;
	filteredImages.emplace_back(
				Filter::Sepia,
				std::make_shared<Image>(
					"imgs/sepia.png",
					std::move(outImageData),
					originalImage->GetWidth(),
					originalImage->GetHeight(),
					originalImage->GetComponents()
				),
				profilerInfo
	);

	HIPCheck(hipFree(d_inImage));

	/**********************************
	******* Padded image filters ******
	**********************************/

	// Box blur
	uVec2 filterDim = {uint32_t(filterParams.m_boxBlur.filterDim), uint32_t(filterParams.m_boxBlur.filterDim)};
	uint32_t paddedWidth, paddedHeight;
	std::vector<uint8_t> paddedImageData = padRepeatEdge(originalImage, filterDim, paddedWidth, paddedHeight);

	pixels = paddedWidth * paddedHeight;
	bytes = sizeof(uint8_t) * pixels * originalImage->GetComponents();

	uint8_t* d_paddedInImage = nullptr;
	HIPCheck(hipMalloc(&d_paddedInImage, bytes));

	HIPCheck(hipEventRecord(memCopyStart));

	HIPCheck(hipMemcpy(d_paddedInImage, paddedImageData.data(), bytes, hipMemcpyHostToDevice));

	HIPCheck(hipEventRecord(memCopyEnd));
	HIPCheck(hipEventSynchronize(memCopyEnd));
	HIPCheck(hipEventElapsedTime(&memCopyElapsed, memCopyStart, memCopyEnd));

	auto paddedImage = std::make_shared<Image>(
				"imgs/repeat_padded.png",
				std::move(paddedImageData),
				paddedWidth,
				paddedHeight,
				originalImage->GetComponents()
	);
	filteredImages.emplace_back(Filter::Padding, paddedImage, ProfilerInfo());

	outImageData = computeBoxBlur(profilerInfo, d_paddedInImage, originalImage, filterDim.x, blockDim, gridDim);
	profilerInfo[ProfilerTimes::MemCopyHostToDevice] = memCopyElapsed;

	std::stringstream ss;
	ss << "imgs/box_blur_" << filterDim.x << "x" << filterDim.x << ".png";
	filteredImages.emplace_back(
				Filter::BoxBlur,
				std::make_shared<Image>(
					ss.str(),
					std::move(outImageData),
					originalImage->GetWidth(),   // The size of the image remains the same
					originalImage->GetHeight(),
					originalImage->GetComponents()
				),
				profilerInfo
	);
	// The next filter may need a different sized padded image, so free the current one.
	HIPCheck(hipFree(d_paddedInImage));

	// Gaus blur
	uVec2 gausFilterDim = {uint32_t(filterParams.m_gausBlur.filterDim), uint32_t(filterParams.m_gausBlur.filterDim)};
	paddedImageData = padRepeatEdge(originalImage, gausFilterDim, paddedWidth, paddedHeight);

	pixels = paddedWidth * paddedHeight;
	bytes = sizeof(uint8_t) * pixels * originalImage->GetComponents();

	d_paddedInImage = nullptr;
	HIPCheck(hipMalloc(&d_paddedInImage, bytes));

	HIPCheck(hipEventRecord(memCopyStart));

	HIPCheck(hipMemcpy(d_paddedInImage, paddedImageData.data(), bytes, hipMemcpyHostToDevice));

	HIPCheck(hipEventRecord(memCopyEnd));
	HIPCheck(hipEventSynchronize(memCopyEnd));
	HIPCheck(hipEventElapsedTime(&memCopyElapsed, memCopyStart, memCopyEnd));

	outImageData = computeGaussianBlur(
			profilerInfo,
			d_paddedInImage,
			originalImage,
			filterParams.m_gausBlur.sigma,
			gausFilterDim.x,
			blockDim,
			gridDim
	);
	profilerInfo[ProfilerTimes::MemCopyHostToDevice] = memCopyElapsed;

	ss.clear();
	ss << "imgs/gaus_blur_" << gausFilterDim.x << "x" << gausFilterDim.x << ".png";
	filteredImages.emplace_back(
				Filter::GaussianBlur,
				std::make_shared<Image>(
					ss.str(),
					std::move(outImageData),
					originalImage->GetWidth(),
					originalImage->GetHeight(),
					originalImage->GetComponents()
				),
				profilerInfo
	);
	HIPCheck(hipFree(d_paddedInImage));

	HIPCheck(hipEventDestroy(memCopyStart));
	HIPCheck(hipEventDestroy(memCopyEnd));

	return filteredImages;
}

namespace detail {

template <class... Args>
std::vector<uint8_t> generalKernelCompute(
		ProfilerInfo& profilerInfo,
		const uint8_t* d_inImage,
		std::shared_ptr<const Image> originalImage,
		uVec3 blockDim,
		uVec3 gridDim,
		GeneralKernelComputeFunction<Args...> kernelComputeFunction,
		Args... args
)
{
	profilerInfo.clear();

	hipEvent_t eventStart, eventEnd;
	HIPCheck(hipEventCreate(&eventStart));
	HIPCheck(hipEventCreate(&eventEnd));

	size_t pixels = originalImage->GetWidth() * originalImage->GetHeight();
	size_t bytes = sizeof(uint8_t) * pixels * originalImage->GetComponents();

	uint8_t* d_outImage = nullptr;
	HIPCheck(hipMalloc(&d_outImage, bytes));

	HIPCheck(hipEventRecord(eventStart, nullptr));

	dim3 hipGridDim = { gridDim.x, gridDim.y, gridDim.z };
	dim3 hipBlockDim = { blockDim.x, blockDim.y, blockDim.z };
	kernelComputeFunction<<<hipGridDim, hipBlockDim, 0, hipStreamDefault>>>(
			d_inImage,
			d_outImage,
			pixels,
			originalImage->GetComponents(),
			args...);
	HIPCheck(hipGetLastError());

	HIPCheck(hipEventRecord(eventEnd, nullptr));
	HIPCheck(hipEventSynchronize(eventEnd));

	float elapsedTime;
	HIPCheck(hipEventElapsedTime(&elapsedTime, eventStart, eventEnd));
	profilerInfo[ProfilerTimes::KernelCompute] = elapsedTime;

	std::vector<uint8_t> outImageData(bytes / sizeof(uint8_t));

	HIPCheck(hipEventRecord(eventStart, nullptr));

	HIPCheck(hipMemcpy(outImageData.data(), d_outImage, bytes, hipMemcpyDeviceToHost));

	HIPCheck(hipEventRecord(eventEnd, nullptr));
	HIPCheck(hipEventSynchronize(eventEnd));
	HIPCheck(hipEventElapsedTime(&elapsedTime, eventStart, eventEnd));
	profilerInfo[ProfilerTimes::MemCopyDeviceToHost] = elapsedTime;

	HIPCheck(hipEventDestroy(eventStart));
	HIPCheck(hipEventDestroy(eventEnd));
	HIPCheck(hipFree(d_outImage));

	return outImageData;
}

std::vector<uint8_t> computeGrayScale(
		ProfilerInfo& profilerInfo,
		const uint8_t* d_inImage,
		std::shared_ptr<const Image> originalImage,
		uVec3 blockDim,
		uVec3 gridDim
)
{
	return generalKernelCompute(
		profilerInfo,
		d_inImage,
		originalImage,
		blockDim,
		gridDim,
		GrayScaleFilter
	);
}

std::vector<uint8_t> computeSepia(
		ProfilerInfo& profilerInfo,
		const uint8_t* d_inImage,
		std::shared_ptr<const Image> originalImage,
		float k,
		uVec3 blockDim,
		uVec3 gridDim
)
{
	return generalKernelCompute(
		profilerInfo,
		d_inImage,
		originalImage,
		blockDim,
		gridDim,
		SepiaFilter,
		k
	);
}

std::vector<uint8_t> nonSeparableKernelCompute(
		ProfilerInfo& profilerInfo,
		const uint8_t* d_paddedInImage,
		std::shared_ptr<const Image> originalImage,
		const float* d_filter,
		uint32_t filterDim,
		uVec3 blockDim,
		uVec3 gridDim
)
{
	profilerInfo.clear();

	hipEvent_t eventStart, eventEnd;
	HIPCheck(hipEventCreate(&eventStart));
	HIPCheck(hipEventCreate(&eventEnd));

	size_t pixels = originalImage->GetWidth() * originalImage->GetHeight();
	size_t bytes = sizeof(uint8_t) * pixels * originalImage->GetComponents();

	uint8_t* d_outImage = nullptr;
	HIPCheck(hipMalloc(&d_outImage, bytes));

	HIPCheck(hipEventRecord(eventStart, nullptr));

	dim3 hipGridDim = { gridDim.x, gridDim.y, gridDim.z };
	dim3 hipBlockDim = { blockDim.x, blockDim.y, blockDim.z };
	NonSeparableConvolution<<<hipGridDim, hipBlockDim, 0, hipStreamDefault>>>(
			d_paddedInImage,
			d_outImage,
			uint2(originalImage->GetWidth(), originalImage->GetHeight()),
			originalImage->GetComponents(),
			d_filter,
			uint2(filterDim, filterDim)
	);
	HIPCheck(hipGetLastError());

	HIPCheck(hipEventRecord(eventEnd, nullptr));
	HIPCheck(hipEventSynchronize(eventEnd));

	float elapsedTime;
	HIPCheck(hipEventElapsedTime(&elapsedTime, eventStart, eventEnd));
	profilerInfo[ProfilerTimes::KernelCompute] = elapsedTime;

	std::vector<uint8_t> outImageData(bytes / sizeof(uint8_t));

	HIPCheck(hipEventRecord(eventStart, nullptr));

	HIPCheck(hipMemcpy(outImageData.data(), d_outImage, bytes, hipMemcpyDeviceToHost));

	HIPCheck(hipEventRecord(eventEnd, nullptr));
	HIPCheck(hipEventSynchronize(eventEnd));

	HIPCheck(hipEventElapsedTime(&elapsedTime, eventStart, eventEnd));
	profilerInfo[ProfilerTimes::MemCopyDeviceToHost] = elapsedTime;

	HIPCheck(hipEventDestroy(eventStart));
	HIPCheck(hipEventDestroy(eventEnd));
	HIPCheck(hipFree(d_outImage));

	return outImageData;
}

std::vector<uint8_t> computeBoxBlur(
		ProfilerInfo& profilerInfo,
		const uint8_t* d_paddedInImage,
		std::shared_ptr<const Image> originalImage,
		uint32_t filterDim,
		uVec3 blockDim,
		uVec3 gridDim
)
{
	size_t filterSize = filterDim * filterDim;
	std::vector<float> filter(filterSize, 1.f / (filterSize));

	float* d_filter = nullptr;
	HIPCheck(hipMalloc(&d_filter, sizeof(float) * filterSize));
	HIPCheck(hipMemcpy(d_filter, filter.data(), sizeof(float) * filterSize, hipMemcpyHostToDevice));

	std::vector<uint8_t> outImageData = nonSeparableKernelCompute(
		profilerInfo,
		d_paddedInImage,
		originalImage,
		d_filter,
		filterDim,
		blockDim,
		gridDim
	);

	HIPCheck(hipFree(d_filter));

	return outImageData;
}

std::vector<uint8_t> computeGaussianBlur(
		ProfilerInfo& profilerInfo,
		const uint8_t* d_paddedInImage,
		std::shared_ptr<const Image> originalImage,
		float sigma,
		uint32_t filterDim,
		uVec3 blockDim,
		uVec3 gridDim
)
{
	auto gausFilter = generateGaussianFilter(sigma, filterDim);

	float* d_filter = nullptr;
	size_t filterSize = filterDim * filterDim;
	HIPCheck(hipMalloc(&d_filter, sizeof(float) * filterSize));
	HIPCheck(hipMemcpy(d_filter, gausFilter.data(), sizeof(float) * filterSize, hipMemcpyHostToDevice));

	std::vector<uint8_t> outImageData = nonSeparableKernelCompute(
		profilerInfo,
		d_paddedInImage,
		originalImage,
		d_filter,
		filterDim,
		blockDim,
		gridDim
	);

	HIPCheck(hipFree(d_filter));

	return outImageData;
}

/**
 * Return a new vector of the image data but padded along each edge with the duplicate of the
 * respective edge.
 * @param image The original unpadded image.
 * @param filterDim The width and height of the kernel filter. Expected to be odd in both
 *                  dimensions.
 * @param[out] width The width of the padded image
 * @param[out] height The height of the padded image
 * @return The edge repeated padded image.
 */
std::vector<uint8_t> padRepeatEdge(std::shared_ptr<const Image> image, uVec2 filterDim, uint32_t& width, uint32_t& height)
{
	std::vector<uint8_t> padded;
	width = height = 0;

	// The filter must be odd else there is no center to the filter.
	if (filterDim.x % 2 == 0 || filterDim.y % 2 == 0) { return padded; }

	int extraCols = filterDim.x / 2;
	int extraRows = filterDim.y / 2;

	width = image->GetWidth() + extraCols * 2;
	height = image->GetHeight() + extraRows * 2;

	size_t pixelX = 0;
	size_t pixelY = 0;
	for (int row = -extraRows; row < image->GetHeight() + extraRows; ++row)
	{
		for (int col = -extraCols; col < image->GetWidth() + extraCols; ++col)
		{
			if (row < 0 && col < 0)
			{
				// Could also mirror the image diagonally but simple take the 0th index for now.
				pixelX = 0;
				pixelY = 0;
			}
			else if (row < 0 && col >= image->GetWidth())
			{
				pixelX = image->GetWidth() - 1;
				pixelY = 0;
			}
			else if (row >= image->GetHeight() && col < 0)
			{
				pixelX = 0;
				pixelY = image->GetHeight() - 1;
			}
			else if (row >= image->GetHeight() && col >= image->GetWidth())
			{
				pixelX = image->GetWidth() - 1;
				pixelY = image->GetHeight() - 1;
			}
			else if (row < 0)
			{
				pixelX = col;
				pixelY = 0;
			}
			else if (row >= image->GetHeight())
			{
				pixelX = col;
				pixelY = image->GetHeight() - 1;
			}
			else if (col < 0)
			{
				pixelX = 0;
				pixelY = row;
			}
			else if (col >= image->GetWidth())
			{
				pixelX = image->GetWidth() - 1;
				pixelY = row;
			}
			else
			{
				pixelX = col;
				pixelY = row;
			}

			for (unsigned int i = 0; i < image->GetComponents(); ++i)
			{
				size_t pixelOffset = (pixelY * image->GetWidth() + pixelX) * image->GetComponents() + i;
				padded.push_back( *(image->GetData() + pixelOffset) );
			}
		}
	}

	return padded;
}

/**
 * Generate the dim x dim gaussian filter.
 * @param sigma The standard deviation
 * @param dim The dimension of the matrix. @note This should be odd. If not then the
 *   returned filter will be of size @c (dim + 1) x (dim + 1)
 */
std::vector<float> generateGaussianFilter(float sigma, uint32_t dim)
{
	std::vector<float> filter;

	if (dim % 2 == 0) { ++dim; }

	uint radius = dim / 2;

	float sum = 0;
	for (uint row = 0; row < dim; ++row)
	{
		for (uint col = 0; col < dim; ++col)
		{
			int x = row - radius;
			int y = col - radius;

			float exp = -(x * x + y * y) / (2.f * sigma * sigma);
			float val = 1.f / (2.f * M_PI * sigma * sigma) * std::pow(M_E, exp);

			filter.push_back(val);
			sum += val;
		}
	}

	for (float& val : filter) { val /= sum; }

	return filter;
}

} // namespace Detail
} // namespace ImageFilter
