#include "ImageFilter.hpp"
#include "ImageFilter_detail.hpp"

#include <cmath>
#include <hip/hip_runtime.h>
#include <memory>

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
		unsigned int components)
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
		float k)
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
 * Performs a convolution on the input image using a non-seperable filter.
 * @param paddedInImage The input image but padded with @c filterDim.x / 2 cols added to each side
 *   and @c filterDim.y / 2 rows added to the top and bottom.
 * @param outImage The image to output to.
 * @param originalDim The dimension of @c paddedInImage were it not padded. Same size as @c outImage.
 * @param components The number of components of @c paddedInImage
 * @param filter A row-major normalized filter.
 * @param The dimensions of @c filter.
 */
__global__ void NonSeperableConvolution(
		const uchar* paddedInImage,
		uchar* outImage,
		uint2 originalDim,
		uint components,
		float* filter,
		uint2 filterDim)
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

std::vector<std::shared_ptr<Image>> GetFilteredImages(std::shared_ptr<const Image> originalImage)
{
	using namespace detail;

	size_t pixels = originalImage->GetWidth() * originalImage->GetHeight();
	size_t bytes = sizeof(uchar) * pixels * originalImage->GetComponents();

	uchar* d_inImage = nullptr;
	HIPCheck(hipMalloc(&d_inImage, bytes));
	HIPCheck(hipMemcpy(d_inImage, originalImage->GetData(), bytes, hipMemcpyHostToDevice));

	constexpr unsigned int threads = 64;
	dim3 blockDim = dim3(threads, 1, 1);
	dim3 gridDim = dim3((pixels + threads - 1) / threads, 1, 1);	// ceiling(pixels / threads)

	std::vector<std::shared_ptr<Image>> filteredImages;
	std::vector<uchar> outImageData;

	outImageData = computeGrayScale(d_inImage, originalImage, blockDim, gridDim);
	filteredImages.push_back(std::make_shared<Image>(
				"imgs/gray.png",
				std::move(outImageData),
				originalImage->GetWidth(),
				originalImage->GetHeight(),
				originalImage->GetComponents())
	);

	outImageData = computeSepia(d_inImage, originalImage, blockDim, gridDim);
	filteredImages.push_back(std::make_shared<Image>(
				"imgs/sepia.png",
				std::move(outImageData),
				originalImage->GetWidth(),
				originalImage->GetHeight(),
				originalImage->GetComponents())
	);

	HIPCheck(hipFree(d_inImage));

	/**********************************
	******* Padded image filters ******
	**********************************/
	uint2 filterDim = {3, 3};
	uint paddedWidth, paddedHeight;
	std::vector<uchar> paddedImageData = padRepeatEdge(originalImage, filterDim, paddedWidth, paddedHeight);

	pixels = paddedWidth * paddedHeight;
	bytes = sizeof(uchar) * pixels * originalImage->GetComponents();

	uchar* d_paddedInImage = nullptr;
	HIPCheck(hipMalloc(&d_paddedInImage, bytes));
	HIPCheck(hipMemcpy(d_paddedInImage, paddedImageData.data(), bytes, hipMemcpyHostToDevice));

	auto paddedImage = std::make_shared<Image>(
				"imgs/repeat_padded.png",
				std::move(paddedImageData),
				paddedWidth,
				paddedHeight,
				originalImage->GetComponents()
	);
	filteredImages.push_back(paddedImage);

	std::vector<float> filter(filterDim.x * filterDim.y, 1.f / (filterDim.x * filterDim.y));
	outImageData = computeBoxBlur(d_paddedInImage, originalImage, filter.data(), filterDim, blockDim, gridDim);
	filteredImages.push_back(std::make_shared<Image>(
				"imgs/box_blur_3x3.png",
				std::move(outImageData),
				originalImage->GetWidth(),    // The size of the image remains the same.
				originalImage->GetHeight(),
				originalImage->GetComponents())
	);

	outImageData = computeGaussianBlur(d_paddedInImage, originalImage, 1.f, filterDim.x, blockDim, gridDim);
	filteredImages.push_back(std::make_shared<Image>(
				"imgs/gaus_blur_3x3.png",
				std::move(outImageData),
				originalImage->GetWidth(),
				originalImage->GetHeight(),
				originalImage->GetComponents())
	);

	HIPCheck(hipFree(d_paddedInImage));

	return filteredImages;
}

namespace detail {
std::vector<uchar> computeGrayScale(const uchar* d_inImage, std::shared_ptr<const Image> originalImage, dim3 blockDim, dim3 gridDim)
{
	size_t pixels = originalImage->GetWidth() * originalImage->GetHeight();
	size_t bytes = sizeof(uchar) * pixels * originalImage->GetComponents();

	uchar* d_outImage = nullptr;
	HIPCheck(hipMalloc(&d_outImage, bytes));

	GrayScaleFilter<<<gridDim, blockDim, 0, hipStreamDefault>>>(d_inImage, d_outImage, pixels, originalImage->GetComponents());
	HIPCheck(hipGetLastError());

	std::vector<uchar> outImageData(bytes / sizeof(uchar));
	HIPCheck(hipMemcpy(outImageData.data(), d_outImage, bytes, hipMemcpyDeviceToHost));
	HIPCheck(hipFree(d_outImage));

	return outImageData;
}

std::vector<uchar> computeSepia(const uchar* d_inImage, std::shared_ptr<const Image> originalImage, dim3 blockDim, dim3 gridDim)
{
	size_t pixels = originalImage->GetWidth() * originalImage->GetHeight();
	size_t bytes = sizeof(uchar) * pixels * originalImage->GetComponents();

	uchar* d_outImage = nullptr;
	HIPCheck(hipMalloc(&d_outImage, bytes));

	SepiaFilter<<<gridDim, blockDim, 0, hipStreamDefault>>>(d_inImage, d_outImage, pixels, originalImage->GetComponents(), 1.f);
	HIPCheck(hipGetLastError());

	std::vector<uchar> outImageData(bytes / sizeof(uchar));
	HIPCheck(hipMemcpy(outImageData.data(), d_outImage, bytes, hipMemcpyDeviceToHost));
	HIPCheck(hipFree(d_outImage));

	return outImageData;
}

std::vector<uchar> computeBoxBlur(
		const uchar* d_paddedInImage,
		std::shared_ptr<const Image> originalImage,
		const float* filter,
		uint2 filterDim,
		dim3 blockDim,
		dim3 gridDim
)
{
	size_t pixels = originalImage->GetWidth() * originalImage->GetHeight();
	size_t bytes = sizeof(uchar) * pixels * originalImage->GetComponents();

	uchar* d_outImage = nullptr;
	HIPCheck(hipMalloc(&d_outImage, bytes));

	float* d_filter = nullptr;
	size_t filterSize = filterDim.x * filterDim.y;
	HIPCheck(hipMalloc(&d_filter, sizeof(float) * filterSize));
	HIPCheck(hipMemcpy(d_filter, filter, sizeof(float) * filterSize, hipMemcpyHostToDevice));

	NonSeperableConvolution<<<gridDim, blockDim, 0, hipStreamDefault>>>(
			d_paddedInImage,
			d_outImage,
			uint2(originalImage->GetWidth(), originalImage->GetHeight()),
			originalImage->GetComponents(),
			d_filter,
			filterDim
	);
	HIPCheck(hipGetLastError());

	std::vector<uchar> outImageData(bytes / sizeof(uchar));
	HIPCheck(hipMemcpy(outImageData.data(), d_outImage, bytes, hipMemcpyDeviceToHost));

	HIPCheck(hipFree(d_filter));
	HIPCheck(hipFree(d_outImage));

	return outImageData;
}

std::vector<uchar> computeGaussianBlur(
		const uchar* d_paddedInImage,
		std::shared_ptr<const Image> originalImage,
		float sigma,
		uint filterDim,
		dim3 blockDim,
		dim3 gridDim)
{
	size_t pixels = originalImage->GetWidth() * originalImage->GetHeight();
	size_t bytes = sizeof(uchar) * pixels * originalImage->GetComponents();

	uchar* d_outImage = nullptr;
	HIPCheck(hipMalloc(&d_outImage, bytes));

	auto gausFilter = generateGaussianFilter(sigma, filterDim);

	float* d_filter = nullptr;
	size_t filterSize = filterDim * filterDim;
	HIPCheck(hipMalloc(&d_filter, sizeof(float) * filterSize));
	HIPCheck(hipMemcpy(d_filter, gausFilter.data(), sizeof(float) * filterSize, hipMemcpyHostToDevice));

	NonSeperableConvolution<<<gridDim, blockDim, 0, hipStreamDefault>>>(
			d_paddedInImage,
			d_outImage,
			uint2(originalImage->GetWidth(), originalImage->GetHeight()),
			originalImage->GetComponents(),
			d_filter,
			uint2(filterDim, filterDim)
	);
	HIPCheck(hipGetLastError());

	std::vector<uchar> outImageData(bytes / sizeof(uchar));
	HIPCheck(hipMemcpy(outImageData.data(), d_outImage, bytes, hipMemcpyDeviceToHost));

	HIPCheck(hipFree(d_filter));
	HIPCheck(hipFree(d_outImage));

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
std::vector<uchar> padRepeatEdge(std::shared_ptr<const Image> image, uint2 filterDim, uint& width, uint& height)
{
	std::vector<uchar> padded;
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
std::vector<float> generateGaussianFilter(float sigma, uint dim)
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
}} // namespace ImageFIlter
