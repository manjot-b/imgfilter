#include <iostream>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Image.hpp"
#include "MainWindow.hpp"

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <image>\n";
		return 1;
	}

	std::string imageFilename(argv[1]);
	std::cout << "Reading in " << imageFilename << '\n';

	Image origImage(imageFilename);

	if (origImage.GetData() == nullptr)
	{
		std::cerr << "Error: Could not read image " << origImage.GetFilename() << '\n';	
		return 1;
	}

	origImage.PrintInfo();

	MainWindow& mainWindow = MainWindow::Get();
	while(mainWindow.ReadyToClose() == false)
	{
		mainWindow.Render();
	}

	mainWindow.Close();
}
