#include <iostream>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
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

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	bool showImGuiDemo = true;
	while(glfwWindowShouldClose(window) == false)
	{
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (showImGuiDemo == true)
		{
			// show ImGui demo
			ImGui::ShowDemoWindow(&showImGuiDemo);
		}

		ImGui::Render();
		GLsizei displayX, displayY;
		glfwGetFramebufferSize(window, &displayX, &displayY);
		glViewport(0, 0, displayX, displayY);
		glClearColor(0.3, 0.2, 0.2, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

		glfwSwapBuffers(window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}
