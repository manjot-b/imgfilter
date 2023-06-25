#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Image.hpp"

static void WindowErrorCallback(int error, const char* description)
{
	std::cerr << "GLFW Error " << error << ": " << description << '\n';
}

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

	glfwSetErrorCallback(WindowErrorCallback);
	if (glfwInit() == false)
	{
		std::cerr << "Error: Could not init GLFW\n";
		return 1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(1280, 720, "Image Manipulator", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cerr << "Error: Could not create GLFW window\n";
		return 1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	while(glfwWindowShouldClose(window) == false)
	{
		glfwPollEvents();

		GLsizei displayX, displayY;
		glfwGetFramebufferSize(window, &displayX, &displayY);
		glViewport(0, 0, displayX, displayY);
		glClearColor(0.3, 0.2, 0.2, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		glfwSwapBuffers(window);
	}
}
