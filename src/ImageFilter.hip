#include "ImageFilter.hpp"

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

std::vector<std::shared_ptr<Image>> GetFilteredImages(std::shared_ptr<Image> originalImage)
{
	std::vector<std::shared_ptr<Image>> filteredImages;

	size_t pixels = originalImage->GetWidth() * originalImage->GetHeight();
	size_t bytes = sizeof(uchar) * pixels * originalImage->GetComponents();

	uchar* inImage = nullptr;
	uchar* outImage = nullptr;

	HIPCheck(hipMalloc(&inImage, bytes));
	HIPCheck(hipMalloc(&outImage, bytes));
	HIPCheck(hipMemcpy(inImage, originalImage->GetData(), bytes, hipMemcpyHostToDevice));

	constexpr unsigned int threads = 64;
	dim3 blockDim = dim3(threads, 1, 1);
	dim3 gridDim = dim3((pixels + threads - 1) / threads, 1, 1);	// ceiling(pixels / threads)

	GrayScaleFilter<<<gridDim, blockDim, 0, hipStreamDefault>>>(inImage, outImage, pixels, originalImage->GetComponents());

	// copy out image to a local buf and create an Image
	std::vector<uchar> grayImage(bytes);
	HIPCheck(hipMemcpy(grayImage.data(), outImage, bytes, hipMemcpyDeviceToHost));

	filteredImages.push_back(std::make_shared<Image>(
				"imgs/gray.png",
				std::move(grayImage),
				originalImage->GetWidth(),
				originalImage->GetHeight(),
				originalImage->GetComponents())
	);

	HIPCheck(hipFree(inImage));
	HIPCheck(hipFree(outImage));

	return filteredImages;
}
}
