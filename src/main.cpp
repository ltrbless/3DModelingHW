#define _TRANSFORMATION_CPP_
#ifdef _TRANSFORMATION_CPP_
//#define _CRT_SECURE_NO_WARNINGS



#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <windows.h>
//#include <gl/GLU.h>
//#include <gl/GL.h>

#include <stb_image.h>
#include <sstream>

#include <iostream>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "Global.h"
#include "UserInterface.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "BrepStruct.h"
#include "Debug.h"

#include <shader.h>
#include <stdio.h>


void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

int main()
{
	//FILE *fp = freopen("test.log", "w", stdout);
	//if (NULL == fp)
	//{
	//	printf("reopen failed\n");
	//	return -1;
	//}

	if (UI::Init() == -1)
	{
		ERROR("Init windows Error!");
		exit(0);
	}

	// build and compile our shader zprogram
	// ------------------------------------
	//Shader lightingShader("../extern/file/light_vs.txt", "../extern/file/light_fs.txt");
	Shader ourShader("../extern/file/shader_vs.txt", "../extern/file/shader_fs.txt");

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	// world space positions of our cubes
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(-0.15f,  -0.15f, 0.05f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};


	unsigned int VBOs[3], VAOs[3];
	glGenVertexArrays(3, VAOs);
	glGenBuffers(3, VBOs);



	while (!glfwWindowShouldClose(globalVar::window))
	{
		// per-frame time logic
	    // --------------------
		float currentFrame = glfwGetTime();
		globalVar::deltaTime = currentFrame - globalVar::lastFrame;
		globalVar::lastFrame = currentFrame;

		/* Swap front and back buffers */
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::SetNextWindowBgAlpha(0.3); // 设置透明度为 0.3

		//if (globalVar::show_demo_window)
			//ImGui::ShowDemoWindow(&globalVar::show_demo_window);


		ImGui::Begin("Brep Operation");                        
		//ImGui::Checkbox("Demo Window", &globalVar::show_demo_window);      // Edit bools storing our window open/close state
		//ImGui::Checkbox("Another Window", &globalVar::show_another_window);
		static bool inputs_step = true;
		const unsigned int unint_one = 1;

		if (ImGui::CollapsingHeader("Help"))
		{
			ImGui::Text("By Taoran Liu");
			ImGui::BulletText("Time: 2021.10.13");
			ImGui::BulletText("Email: taoranliu@zju.edu.cn");
			ImGui::BulletText("Phone: 19550210571");
			ImGui::Separator();

			ImGui::Text("USER GUIDE:");
			ImGui::BulletText("Press 'ESC' to close current window.");
			ImGui::BulletText("When you right-click one you can drag the model and twice is quit.");
			ImGui::BulletText("When you middle-click one you can scale the model and twice is quit.");
			ImGui::Separator();

			//ImGui::ShowUserGuide("");
		}


		if (ImGui::CollapsingHeader("Tool"))
		{
			if (ImGui::Button("Demo example"))
			{
				INFO_log("Demo Show !");
				globalVar::Euler.clearn();
				globalVar::Euler.exampleEulerOp();
			}
			ImGui::SameLine();
			if (ImGui::Button("Demo example 2"))
			{
				INFO_log("Demo Show !");
				globalVar::Euler.clearn();
				globalVar::Euler.exampleEulerOp2();
			}
			ImGui::SameLine();

			if (globalVar::wireFrame == 0)
			{
				if (ImGui::Button("WireFrame"))
				{
					INFO_log("Wire Frame Model.");
					globalVar::wireFrame = 1;
				}
			}
			else
			{
				if (ImGui::Button("SolidFrame"))
				{
					INFO_log("Solid Frame Model.");
					globalVar::wireFrame = 0;
				}
			}


			ImGui::SameLine();
			if (ImGui::Button("Clearn"))
			{
				INFO_log("Clearn current model.");
				// 释放内存
				globalVar::Euler.solid = nullptr;
				delete globalVar::vertexs;
				globalVar::vertexs = nullptr;
				delete globalVar::edges;
				globalVar::edges = nullptr;
				delete globalVar::face;
				globalVar::face = nullptr;
				globalVar::Euler.clearn();
				//globalVar::Euler.clearn();
			}
			ImGui::SameLine();
			if (ImGui::Button("Print All Face"))
			{
				globalVar::Euler.PrintAllFace();
			}
		}


		if(ImGui::CollapsingHeader("mvfs and mev operation"))
		{
			static int counter = 0;
			const float f32_one = 1.0f;
			static float  f32_x = 0.0f;
			static float  f32_y = 0.0f;
			static float  f32_z = 0.0f;
			static unsigned int vertexId_mev = 0;
			static unsigned int loopId_mev = 0;

			ImGui::Text("Inputs coordinates of point.");
			ImGui::Separator();
			ImGui::Checkbox("Show step buttons", &inputs_step);
			ImGui::InputScalar("input x coord", ImGuiDataType_Float, &f32_x, inputs_step ? &f32_one : NULL);
			ImGui::InputScalar("input y coord", ImGuiDataType_Float, &f32_y, inputs_step ? &f32_one : NULL);
			ImGui::InputScalar("input z coord", ImGuiDataType_Float, &f32_z, inputs_step ? &f32_one : NULL);
			ImGui::Separator();

			if (ImGui::Button("mvfs"))
			{ 
				Point curPoint = Point(f32_x, f32_y, f32_z);
				globalVar::Euler.solid = globalVar::Euler.mvfs(curPoint);
			}
			ImGui::Separator();

			ImGui::InputScalar("input vertex id", ImGuiDataType_U32, &vertexId_mev, inputs_step ? &unint_one : NULL);
			ImGui::InputScalar("input loop id mev", ImGuiDataType_U32, &loopId_mev, inputs_step ? &unint_one : NULL);
			if (globalVar::Euler.solid != nullptr && ImGui::Button("mev"))
			{
				Point curPoint = Point(f32_x, f32_y, f32_z);
				globalVar::Euler.mev(Vertex::vertexVec[vertexId_mev], curPoint, Loop::loopVec[loopId_mev]);
				vertexId_mev = Vertex::vertexVec.size() - 1;
			}
			//ImGui::SliderFloat("float", &globalVar::f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
	
		}
		if ( ImGui::CollapsingHeader("mef and kemr operation."))
		{
			static unsigned int vertexId_mk = 0;
			static unsigned int vertexId_mk2 = 0;
			static unsigned int loopId_mk = 0;
			ImGui::InputScalar("input vertex 1 id", ImGuiDataType_U32, &vertexId_mk, inputs_step ? &unint_one : NULL);
			ImGui::InputScalar("input vertex 2 id", ImGuiDataType_U32, &vertexId_mk2, inputs_step ? &unint_one : NULL);
			ImGui::InputScalar("input loop id mef&kemr", ImGuiDataType_U32, &loopId_mk, inputs_step ? &unint_one : NULL);
			ImGui::Separator();
			if (ImGui::Button("mef"))
			{
				if (globalVar::Euler.solid != nullptr && Vertex::vertexVec.size() >= 2)
					globalVar::Euler.mef(Vertex::vertexVec[vertexId_mk], Vertex::vertexVec[vertexId_mk2], Loop::loopVec[loopId_mk]);
				else
					INFO_log("Please create a solid first.");
			}
			ImGui::SameLine();
			if (ImGui::Button("kemr"))
			{
				if(globalVar::Euler.solid != nullptr && Vertex::vertexVec.size() >= 2)
					globalVar::Euler.kemr(Vertex::vertexVec[vertexId_mk], Vertex::vertexVec[vertexId_mk2], Loop::loopVec[loopId_mk]);
				else
				{
					INFO_log("PLease create a solid first.");
				}
			}
		}
		if (ImGui::CollapsingHeader("kfmrh operation."))
		{
			static unsigned int loopId_1 = 0;
			static unsigned int loopId_2 = 0;
			ImGui::InputScalar("input Outloop id", ImGuiDataType_U32, &loopId_1, inputs_step ? &unint_one : NULL);
			ImGui::InputScalar("input Innerloop id", ImGuiDataType_U32, &loopId_2, inputs_step ? &unint_one : NULL);
			if (ImGui::Button("kemrh"))
			{
				if (globalVar::Euler.solid != nullptr && Vertex::vertexVec.size() >= 2)
				{
					globalVar::Euler.kfmrh(Loop::loopVec[loopId_1], Loop::loopVec[loopId_2]);
				}
			}
		}

		if (ImGui::CollapsingHeader("sweep operation."))
		{
			static unsigned int loopId_s = 0;
			static Eigen::Vector3d dir = Eigen::Vector3d(0, 0, -1);
			static double dis = 1.0;
			ImGui::InputScalar("input loop id to sweep", ImGuiDataType_U32, &loopId_s, inputs_step ? &unint_one : NULL);
			ImGui::InputScalar("input x direction", ImGuiDataType_Double, &dir[0],  NULL);
			ImGui::InputScalar("input y direction", ImGuiDataType_Double, &dir[1],NULL);
			ImGui::InputScalar("input z direction", ImGuiDataType_Double, &dir[2],  NULL);
			ImGui::InputScalar("input distance", ImGuiDataType_Double, &dis, inputs_step ? &unint_one : NULL);
			if (ImGui::Button("sweep"))
			{
				if (globalVar::Euler.solid != nullptr && Vertex::vertexVec.size() >= 2)
				{
					globalVar::Euler.sweep(Loop::loopVec[loopId_s], dir, dis);
				}
			}
		}

		if (ImGui::CollapsingHeader("Log"))
		{
			static int test_type = 0;
			static ImGuiTextBuffer log;
			static int lines = 0;
			if (ImGui::Button("Clear log")) {
				errno_t err;
				FILE *fp = nullptr;
				err = fopen_s(&fp, "Info.log", "w");
				fclose(fp);
				log.clear();
				lines = 0;
			}

			ImGui::Separator();
			std::ifstream info("Info.log");
			std::string data = "";
			for(int i = 0;std::getline(info, data);i++)
			{
				log.appendf("%s\n",data.c_str());
				data.clear();
			}
			info.close();


			ImGui::BeginChild("Log");
			ImGui::TextUnformatted(log.begin(), log.end());
			log.clear();
			ImGui::EndChild();
		}

		ImGui::End();

	
		if(globalVar::Euler.solid != nullptr)
		globalVar::Euler.GetAllVertex(&(globalVar::vertexs));
		if (globalVar::vertexs != nullptr)
		{
			glBindVertexArray(VAOs[0]);
			glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
			//glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float), vertexs, GL_STATIC_DRAW);
			glBufferData(GL_ARRAY_BUFFER, Vertex::vertexNum * 3 * sizeof(float), globalVar::vertexs, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);

			for (int i = 0; i < Vertex::vertexVec.size(); i++)
			{
				globalVar::maxxVec.x = max(globalVar::maxxVec.x, Vertex::vertexVec[i]->point.coord[0]);
				globalVar::maxxVec.y = max(globalVar::maxxVec.y, Vertex::vertexVec[i]->point.coord[1]);
				globalVar::maxxVec.z = max(globalVar::maxxVec.z, Vertex::vertexVec[i]->point.coord[2]);

				globalVar::minnVec.x = min(globalVar::minnVec.x, Vertex::vertexVec[i]->point.coord[0]);
				globalVar::minnVec.y = min(globalVar::minnVec.y, Vertex::vertexVec[i]->point.coord[1]);
				globalVar::minnVec.z = min(globalVar::minnVec.z, Vertex::vertexVec[i]->point.coord[2]);
			}


		}
		
		//float *edges;
		if(globalVar::Euler.solid != nullptr)
		globalVar::Euler.GetAllEdge(&(globalVar::edges));
		if (globalVar::edges != nullptr)
		{
			glBindVertexArray(VAOs[1]);
			glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
			glBufferData(GL_ARRAY_BUFFER, globalVar::Euler.solid->edgeNum * 2 * 3 * sizeof(float), globalVar::edges, GL_STATIC_DRAW);
			// position attribute
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
		}

		//float* face;
		if (globalVar::Euler.solid != nullptr && globalVar::curLoopNum != Loop::loopVec.size())
		{
			globalVar::faceNum = globalVar::Euler.CreateAllFaceToView(&(globalVar::face));
			INFO_log("Here !!!");
			INFO_log("The number of face is %d", globalVar::faceNum);
			globalVar::curLoopNum = Loop::loopVec.size();
		}
		if (globalVar::face != nullptr)
		{
			glBindVertexArray(VAOs[2]);
			glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);
			//glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float), vertexs, GL_STATIC_DRAW);
			glBufferData(GL_ARRAY_BUFFER, globalVar::faceNum * 3 * sizeof(float), globalVar::face, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);

		}
		// input
		// -----
		processInput(globalVar::window);

		// render
		// ------
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// be sure to activate shader when setting uniforms/drawing objects
		//lightingShader.use();
		//lightingShader.setVec3("objectColor", 0.0f, 0.0f, 0.0f);
		//lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		// lighting
		//glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
		//lightingShader.setVec3("lightPos", lightPos);


		// activate shader
		ourShader.use();

		glm::mat4 model = glm::mat4(1.0f);
	/*	cubePositions[1][0] = (globalVar::maxxVec[0] + globalVar::minnVec[0]) / 2.0;
		cubePositions[1][1] = (globalVar::maxxVec[1] + globalVar::minnVec[1]) / 2.0;
		cubePositions[1][2] = (globalVar::maxxVec[2] + globalVar::minnVec[2]) / 2.0;*/
		glm::vec3 curPos = (globalVar::maxxVec + globalVar::minnVec);
		curPos *= -1;
		curPos /= 20.0; // normalize coord
		model = glm::translate(model, curPos); // adaptive judge position
		ourShader.setMat4("model", model);
		//lightingShader.setMat4("model", model);

		// pass projection matrix to shader (note that in this case it could change every frame)
		glm::mat4 projection = glm::perspective(glm::radians(globalVar::fov), (float)globalVar::SCR_WIDTH / (float)globalVar::SCR_HEIGHT, 0.1f, 100.0f);
		ourShader.setMat4("projection", projection);
		//lightingShader.setMat4("projection", projection);
	

		// camera/view transformation
		glm::mat4 view = glm::lookAt(globalVar::cameraPos, glm::vec3(0.0, 0.0, 0.0), globalVar::cameraUp);
		ourShader.setMat4("view", view);
		//lightingShader.setMat4("view", view);

		ourShader.use();
		ourShader.setVec4("aColor", glm::vec4(0.458f, 0.4745f, 0.290f, 1.0f));

		glEnable(GL_LINE_SMOOTH);

		if (globalVar::vertexs != nullptr)
		{
			glBindVertexArray(VAOs[0]);
			glPointSize(6);
			glDrawArrays(GL_POINTS, 0, Vertex::vertexNum);

		}

		ourShader.use();
		ourShader.setVec4("aColor", glm::vec4(0.27f, 0.537f, 0.580f, 1.0f));
		if (globalVar::edges != nullptr)
		{
			glBindVertexArray(VAOs[0]);
			glBindVertexArray(VAOs[1]);
			glLineWidth(3);
			glDrawArrays(GL_LINES, 0, globalVar::Euler.solid->edgeNum * 2);

		}

		ourShader.use();
		//ourShader.setVec4("aColor", glm::vec4(0.0f, 0.0f, 0.0f, 0.5f));
		ourShader.setVec4("aColor", glm::vec4(0.69f, 0.784f, 0.733f, 0.5f));
		if(globalVar::face != nullptr && globalVar::wireFrame == 0)
		{
			glBindVertexArray(VAOs[0]);
			glBindVertexArray(VAOs[2]);
			//glLineWidth(2);
			glDrawArrays(GL_TRIANGLES, 0, globalVar::faceNum * 3);
		}


		 //Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(globalVar::window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(3, VAOs);
	glDeleteBuffers(3, VBOs);

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------


#endif // _TRANSFORMATION_CPP_
