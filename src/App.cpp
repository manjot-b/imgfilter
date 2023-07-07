#include "App.hpp"
#include "PreviewWindow.hpp"

#include <iostream>

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
		m_mainWindow.Render();
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

	m_mainWindow.GetCanvasWindow()->SetImage(m_originalImage);
	m_mainWindow.GetPreviewWindow()->GetThumbnails().emplace_back("Original", m_originalImage);

	return true;
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
