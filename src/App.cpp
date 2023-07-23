#include "App.hpp"

#include <filesystem>
#include <iostream>

#include "ImageFilter.hpp"
#include "PreviewWindow.hpp"

App::App() : m_mainWindow(MainWindow::Get())
{
	// Create the main window so the the GLFW and OpenGL contexts
	// get created before using them.
}

App& App::Get()
{
	static App app;
	return app;
}

void App::Run()
{
	while(m_mainWindow.ReadyToClose() == false)
	{
		m_mainWindow.Render(m_filterParams);

		if (m_filterParams != m_prevfilterParams)
		{
			ComputeAndDisplayFilteredImages();
			m_prevfilterParams = m_filterParams;
		}
	}

	m_mainWindow.Close();
}

bool App::LoadImage(const std::string& filepath)
{
	m_originalImage = std::make_shared<Image>(filepath);

	if (m_originalImage->GetData() == nullptr)
	{
		return false;
	}

	m_originalImage->PrintInfo();

	ComputeAndDisplayFilteredImages();

	return true;
}

void App::ComputeAndDisplayFilteredImages()
{
	std::filesystem::path originalPath(m_originalImage->GetFilename());
	m_mainWindow.GetCanvasWindow()->SetImage(originalPath.stem(), m_originalImage);

	m_mainWindow.GetPreviewWindow()->GetThumbnails().clear();
	m_mainWindow.GetPreviewWindow()->GetThumbnails().emplace_back(originalPath.stem(), m_originalImage);

	// Extract the stem of the filename from the image. Then, use it to create a thumbnail.
	// The stem is expected to be unique.
	auto filteredImages = ImageFilter::GetFilteredImages(m_originalImage, m_filterParams);
	for (auto& image : filteredImages)
	{
		std::filesystem::path path(image->GetFilename());
		m_mainWindow.GetPreviewWindow()->GetThumbnails().emplace_back(path.stem(), image);
	}

}

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <image>\n";
		return 1;
	}

	// Create the app so the the GLFW and OpenGL contexts
	// get created before using them.
	App& app = App::Get();

	std::string imageFilename(argv[1]);
	std::cout << "Reading in " << imageFilename << '\n';
	if (app.LoadImage(imageFilename) == false)
	{
		std::cerr << "Error: Could not read image " << imageFilename << '\n';	
	}

	app.Run();
}
