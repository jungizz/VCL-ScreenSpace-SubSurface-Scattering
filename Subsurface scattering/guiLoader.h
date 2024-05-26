#pragma once

#ifndef guiLoader_h
#define guiLoader_h

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include "objLoader.h"

int selectModel = 0;
int selectScene = 0;

float kernelParam = 2.0f;
float specReflectance = 0.028;

bool isAutoRotate = false;

void guiInit(GLFWwindow* window, const char* glsl_version) {

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	
	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init(glsl_version);
}

//void guiRender(GLFWwindow* window) {
//    // Start the Dear ImGui frame
//    ImGui_ImplOpenGL3_NewFrame();
//    ImGui_ImplGlfw_NewFrame();
//    ImGui::NewFrame();
//
//    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
//    {
//
//        ImGui::Begin("Setting");
//
//        ImGui::Text("Select Model");
//        if (ImGui::RadioButton("Head", &selectModel, 0)) init();
//        if (ImGui::RadioButton("Hand", &selectModel, 1)) init();
//
//        ImGui::Text("Select Scene");
//        ImGui::RadioButton("Default Rendering", &selectScene, 0);
//        ImGui::RadioButton("Subsurface Scattering", &selectScene, 1);
//        ImGui::RadioButton("Gaussian blur on Diffuse", &selectScene, 2);
//
//        ImGui::Text("Property");
//        ImGui::Checkbox("Kernel with Depth", &isAdjKernel);
//        ImGui::InputFloat3("specular reflectance", &specReflectance.x);
//
//        ImGui::End();
//    }
//
//
//    // Rendering
//    ImGui::Render();
//
//    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//
//}


#endif
