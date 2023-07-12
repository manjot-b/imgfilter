#include "ImageFilter.hpp"

#include <hip/hip_runtime.h>
#include <memory>

__global__ void GrayScaleFilter(
		const uchar* inImage,
		uchar* outImage,
		unsigned int pixels,
		unsigned int components)
{
	const unsigned int PIXEL_IDX = (blockIdx.x * blockDim.x + threadIdx.x) * components;
	const unsigned int TOTAL_BYTES = pixels * components;

	if (PIXEL_IDX < TOTAL_BYTES)
	{
		uchar3 color = {0, 0, 0};
		color.x = inImage[PIXEL_IDX + 0];
		color.y = inImage[PIXEL_IDX + 1];
		color.z = inImage[PIXEL_IDX + 2];

		const uchar RGBComponents = 3;
		uchar gray = (color.x + color.y + color.z) / RGBComponents;

		for (int i = 0; i < RGBComponents; i++)	// Assume image is RGB or RGBA
		{
			outImage[PIXEL_IDX + i] = gray;
		}
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

	// TODO: Modify stb_image to use new() instead of malloc(). Then modify Image so that it takes
	// ownership of the pointer passed in its constructor rather than performing a memcpy. This way
	// the destructor of Image can always call delete[];
	// copy out image to a local buf and create an Image
	uchar* grayImage = new uchar[bytes];
	HIPCheck(hipMemcpy(grayImage, outImage, bytes, hipMemcpyDeviceToHost));
	filteredImages.push_back(std::make_shared<Image>(
				"imgs/gray.jpg",
				grayImage,
				originalImage->GetWidth(),
				originalImage->GetHeight(),
				originalImage->GetComponents())
	);
	delete[] grayImage;

	HIPCheck(hipFree(inImage));
	HIPCheck(hipFree(outImage));

	return filteredImages;
}
}
