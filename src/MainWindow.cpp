#include "MainWindow.hpp"

#include <iostream>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

MainWindow& MainWindow::Get()
{
	static MainWindow mainWindow;

	return mainWindow;
}

MainWindow::MainWindow() :
	IWindow("DockSpaceWindow"),
	m_canvasWindow(std::make_shared<CanvasWindow>()),
	m_previewWindow(std::make_shared<PreviewWindow>()),
	m_profilerWindow(std::make_shared<ProfilerWindow>())
{
	m_windows.push_back(std::static_pointer_cast<IWindow>(m_canvasWindow));
	m_windows.push_back(std::static_pointer_cast<IWindow>(m_previewWindow));
	m_windows.push_back(std::static_pointer_cast<IWindow>(m_profilerWindow));

	glfwSetErrorCallback(MainWindow::ErrorCallback);

	if (glfwInit() == false)
	{
		std::cerr << "Error: Could not init GLFW\n";
		exit(1);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	m_window = glfwCreateWindow(1280, 720, "Image Manipulator", nullptr, nullptr);
	if (m_window == nullptr)
	{
		std::cerr << "Error: Could not create GLFW window\n";
		exit(1);
	}

	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(1);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui_ImplGlfw_InitForOpenGL(m_window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

void MainWindow::Close()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(m_window);
	glfwTerminate();
}

void MainWindow::Render()
{
	glfwPollEvents();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	displayDockingSpace();

	for (auto window : m_windows)
	{
		window->Render();
	}

	bool showImGuiDemo = false;
	if (showImGuiDemo == true)
	{
		// show ImGui demo
		ImGui::ShowDemoWindow(&showImGuiDemo);
	}

	ImGui::Render();
	GLsizei displayX, displayY;
	glfwGetFramebufferSize(m_window, &displayX, &displayY);
	glViewport(0, 0, displayX, displayY);
	glClearColor(0.3, 0.2, 0.2, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Update and Render additional Platform Windows
	// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
	//  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}

	glfwSwapBuffers(m_window);
}

void MainWindow::displayDockingSpace()
{
    ImGuiDockNodeFlags dockSpaceFlags = ImGuiDockNodeFlags_DockSpace;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	const ImGuiViewport* viewport = ImGui::GetMainViewport();

	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, .0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, .0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(.0f, .0f));

	windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::Begin(m_windowName.c_str(), nullptr, windowFlags);
	ImGui::PopStyleVar(3);

    // Submit the DockSpace
	ImGuiID dockSpaceID = ImGui::GetID("MyDockSpace");
	ImGui::DockSpace(dockSpaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

	// On start up dock the windows to their starting location
    static bool firstTimeDocking = true;
    if (firstTimeDocking) {
        firstTimeDocking = false;
        ImGui::DockBuilderRemoveNode(dockSpaceID);
        ImGui::DockBuilderAddNode(dockSpaceID, dockSpaceFlags);
        ImGui::DockBuilderSetNodeSize(dockSpaceID, viewport->Size);

        ImGuiID leftDockID = ImGui::DockBuilderSplitNode(dockSpaceID, ImGuiDir_Left, 0.2f, nullptr, &dockSpaceID);
        ImGuiID downDockID;
		ImGuiID upDockID = ImGui::DockBuilderSplitNode(dockSpaceID, ImGuiDir_Up, 0.75f, nullptr, &downDockID);

        ImGui::DockBuilderDockWindow(m_canvasWindow->GetWindowName(), upDockID);
        ImGui::DockBuilderDockWindow(m_previewWindow->GetWindowName(), downDockID);
        ImGui::DockBuilderDockWindow(m_profilerWindow->GetWindowName(), leftDockID);

        ImGui::DockBuilderFinish(dockSpaceID);
    }

    ImGui::End();
}

void MainWindow::ErrorCallback(int error, const char* description)
{
	std::cerr << "GLFW Error " << error << ": " << description << '\n';
}

bool MainWindow::ReadyToClose()
{
	return glfwWindowShouldClose(m_window);
}

bool MainWindow::LoadImage(const std::string& filepath)
{
	m_originalImage = std::make_shared<Image>(filepath);

	if (m_originalImage->GetData() == nullptr)
	{
		return false;
	}

	m_originalImage->PrintInfo();

	m_canvasWindow->SetImage(m_originalImage);

	return true;
}
