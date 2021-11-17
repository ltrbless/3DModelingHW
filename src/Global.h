#pragma once

#include "BrepStruct.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <string>

#define max(a,b) a > b ? a : b
#define min(a,b) a < b ? a : b


class globalVar
{
public:
	static const unsigned int SCR_WIDTH;
	static const unsigned int SCR_HEIGHT;

	static bool firstMouse;
	static float yaw;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
	static float pitch;
	static float lastX;
	static float lastY;

	static bool show_demo_window;
	static bool show_another_window;
	static float f;

	static int leftMouse;
	static int middleMouse;
	static float fov;

	static float deltaTime;// time between current frame and last frame
	static float lastFrame;

	// camera
	static glm::vec3 cameraPos; // 相机的坐标原点
	static glm::vec3 cameraFront; // 相机的朝向
	static glm::vec3 cameraUp; // 

	static EulerOp Euler;

	static GLFWwindow* window;

	static std::vector<std::string> logVec;

	static float* vertexs;
	static float* edges;
	static float* face;

	static int curLoopNum;
	static int faceNum;

	static glm::vec3 maxxVec;
	static glm::vec3 minnVec;

	static bool wireFrame;

};


// settings
const unsigned int globalVar::SCR_WIDTH = 800;
const unsigned int globalVar::SCR_HEIGHT = 600;

bool globalVar::firstMouse = true;
float globalVar::yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float globalVar::pitch = 0.0f;
float globalVar::lastX = 800.0f / 2.0;
float globalVar::lastY = 600.0 / 2.0;

// render loop
// -----------
bool globalVar::show_demo_window = false;
bool globalVar::show_another_window = false;
float globalVar::f = 0.0f;

int globalVar::leftMouse = 0;
int globalVar::middleMouse = 0;
float globalVar::fov = 45.0f;

// timing
float globalVar::deltaTime = 0.0f;	// time between current frame and last frame
float globalVar::lastFrame = 0.0f;

// camera
glm::vec3 globalVar::cameraPos = glm::vec3(0.0f, 0.0f, 3.0f); // 相机的坐标原点
glm::vec3 globalVar::cameraFront = glm::vec3(0.0f, 0.0f, 0.0f); // 相机的朝向
glm::vec3 globalVar::cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); // 

EulerOp globalVar::Euler;

GLFWwindow* globalVar::window = nullptr;

std::vector<std::string> globalVar::logVec;

float * globalVar::vertexs = nullptr;
float * globalVar::edges = nullptr;
float * globalVar::face = nullptr;

int globalVar::curLoopNum = 0;
int globalVar::faceNum = 0;

glm::vec3 globalVar::maxxVec = glm::vec3(-100, -100, -100);
glm::vec3 globalVar::minnVec = glm::vec3(100, 100, 100);

bool globalVar::wireFrame = false;
