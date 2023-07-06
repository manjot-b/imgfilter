#include <iostream>

#include "MainWindow.hpp"

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <image>\n";
		return 1;
	}

	// Create the main window so the the GLFW and OpenGL contexts
	// get created before using them.
	MainWindow& mainWindow = MainWindow::Get();

	std::string imageFilename(argv[1]);
	std::cout << "Reading in " << imageFilename << '\n';
	if (mainWindow.LoadImage(imageFilename) == false)
	{
		std::cerr << "Error: Could not read image " << imageFilename << '\n';	
	}

	while(mainWindow.ReadyToClose() == false)
	{
		mainWindow.Render();
	}

	mainWindow.Close();
}
