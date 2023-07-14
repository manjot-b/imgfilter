#include "ImageFilter.hpp"
#include "ImageFilter_internal.hpp"

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

	if (PIXEL_OFFSET < TOTAL_BYTES)
	{
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

	if (PIXEL_OFFSET < TOTAL_BYTES)
	{
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

	return filteredImages;
}

std::vector<uchar> computeGrayScale(const uchar* d_inImage, const std::shared_ptr<const Image> originalImage, dim3 blockDim, dim3 gridDim)
{
	size_t pixels = originalImage->GetWidth() * originalImage->GetHeight();
	size_t bytes = sizeof(uchar) * pixels * originalImage->GetComponents();

	uchar* d_outImage = nullptr;
	HIPCheck(hipMalloc(&d_outImage, bytes));

	GrayScaleFilter<<<gridDim, blockDim, 0, hipStreamDefault>>>(d_inImage, d_outImage, pixels, originalImage->GetComponents());
	HIPCheck(hipGetLastError());

	std::vector<uchar> outImageData(bytes);
	HIPCheck(hipMemcpy(outImageData.data(), d_outImage, bytes, hipMemcpyDeviceToHost));
	HIPCheck(hipFree(d_outImage));

	return outImageData;
}

std::vector<uchar> computeSepia(const uchar* d_inImage, const std::shared_ptr<const Image> originalImage, dim3 blockDim, dim3 gridDim)
{
	size_t pixels = originalImage->GetWidth() * originalImage->GetHeight();
	size_t bytes = sizeof(uchar) * pixels * originalImage->GetComponents();

	uchar* d_outImage = nullptr;
	HIPCheck(hipMalloc(&d_outImage, bytes));

	SepiaFilter<<<gridDim, blockDim, 0, hipStreamDefault>>>(d_inImage, d_outImage, pixels, originalImage->GetComponents(), 1.f);
	HIPCheck(hipGetLastError());

	std::vector<uchar> outImageData(bytes);
	HIPCheck(hipMemcpy(outImageData.data(), d_outImage, bytes, hipMemcpyDeviceToHost));
	HIPCheck(hipFree(d_outImage));

	return outImageData;
}
}
