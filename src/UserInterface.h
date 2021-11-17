#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "Global.h"

namespace UI
{

	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	void scroll_callback_empty(GLFWwindow* window, double xoffset, double yoffset);
	void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	void mouse_callback_empty(GLFWwindow* window, double xpos, double ypos);

	int Init();

}

int UI::Init()
{
	// glfw: initialize and configure
// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	globalVar::window = glfwCreateWindow(globalVar::SCR_WIDTH, globalVar::SCR_HEIGHT, "3D Modeling By ltr", NULL, NULL);
	if (globalVar::window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(globalVar::window);
	glfwSetFramebufferSizeCallback(globalVar::window, UI::framebuffer_size_callback);
	glfwSetMouseButtonCallback(globalVar::window, UI::mouse_button_callback);


	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	// configure global opengl state
// -----------------------------
	glEnable(GL_DEPTH_TEST);

	//创建并绑定ImGui
	ImGui::CreateContext();     // Setup Dear ImGui context
	ImGui::StyleColorsDark();       // Setup Dear ImGui style
	ImGui_ImplGlfw_InitForOpenGL(globalVar::window, true);     // Setup Platform/Renderer backends
	ImGui_ImplOpenGL3_Init("#version 450");
	//ImGuiIO &io = ImGui::GetIO();
	//(void)io;

	return 1;

}


void UI::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (button)
		{
		case GLFW_MOUSE_BUTTON_RIGHT:
			globalVar::leftMouse = 1 - globalVar::leftMouse;
			if (globalVar::leftMouse) {
				glfwSetCursorPosCallback(globalVar::window, mouse_callback);
				glfwSetInputMode(globalVar::window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
			if (!globalVar::leftMouse) {
				glfwSetCursorPosCallback(globalVar::window, mouse_callback_empty);
				//glfwSetInputMode(globalVar::window, GLFW_CURSOR_DISABLED, GLFW_FALSE);
				glfwSetInputMode(globalVar::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			globalVar::middleMouse = 1 - globalVar::middleMouse;
			if (globalVar::middleMouse)
			{
				glfwSetScrollCallback(globalVar::window, UI::scroll_callback);
			}
			else
			{
				glfwSetScrollCallback(globalVar::window, UI::scroll_callback_empty);
			}
			break;
		case GLFW_MOUSE_BUTTON_LEFT:
			break;
		default:
			return;
		}
	}
	return;
}
// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void UI::scroll_callback_empty(GLFWwindow* window, double xoffset, double yoffset) 
{
	
}

void UI::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	globalVar::fov -= (float)yoffset;
	if (globalVar::fov < 1.0f)
		globalVar::fov = 1.0f;
	if (globalVar::fov > 45.0f)
		globalVar::fov = 45.0f;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void UI::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void UI::mouse_callback_empty(GLFWwindow* window, double xpos, double ypos)
{

}


void UI::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (globalVar::firstMouse)
	{
		globalVar::lastX = xpos;
		globalVar::lastY = ypos;
		globalVar::firstMouse = false;
		return;
	}

	float xoffset = xpos - globalVar::lastX;
	float yoffset = globalVar::lastY - ypos; // reversed since y-coordinates go from bottom to top
	globalVar::lastX = xpos;
	globalVar::lastY = ypos;

	float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	globalVar::yaw += xoffset;
	globalVar::pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (globalVar::pitch > 89.0f)
		globalVar::pitch = 89.0f;
	if (globalVar::pitch < -89.0f)
		globalVar::pitch = -89.0f;

	//glm::vec3 front;
	//front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	//front.y = sin(glm::radians(pitch));
	//front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	//cameraFront = glm::normalize(front);
	glm::vec3 posTmp;
	posTmp.x = cos(glm::radians(-globalVar::pitch)) * sin(glm::radians(-globalVar::yaw)) * 5;
	posTmp.z = cos(glm::radians(-globalVar::pitch)) * cos(glm::radians(-globalVar::yaw)) * 5;
	posTmp.y = sin(glm::radians(-globalVar::pitch)) * 5;
	globalVar::cameraPos = posTmp;
}
