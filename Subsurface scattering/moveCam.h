//
//  Created by Song Jungeun on 2024/1/4
//

#pragma once

#ifndef moveCam_h
#define moveCam_h

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

using namespace glm;

const float PI = 3.14159265358979f;

//float scaleFactor = 1.0;
//bool isShrinking = true;
//float theta = 0;
float cameraDistance = 2;

float cameraTheta = 0;      // ���� ȸ�� ����
float cameraPhi = 0;        // ���� ȸ�� ����
float fovy = 30 * PI / 180; // �þ߰�


void cursorPosCallback(GLFWwindow* win, double xpos, double ypos) {
    static double lastX = 0;
    static double lastY = 0;
    if (glfwGetMouseButton(win, GLFW_MOUSE_BUTTON_1)) {
        double dx = xpos - lastX;
        double dy = ypos - lastY;
        int w, h;
        glfwGetWindowSize(win, &w, &h);
        cameraTheta -= dx / w * PI; // ���콺 x�� �̵����� PI ���Ͽ� ���� ������ ��ȯ
        cameraPhi -= dy / h * PI;   // ���콺 y�� �̵����� PI ���Ͽ� ���� ������ ��ȯ
        if (cameraPhi < -PI / 2 + 0.01) {
            cameraPhi = -PI / 2 + 0.01;
        }
        else if (cameraPhi > PI / 2 - 0.01) {
            cameraPhi = PI / 2 - 0.01;
        }
    }
    lastX = xpos;
    lastY = ypos;
}

void scrollCallback(GLFWwindow* win, double xoffset, double yoffset) {
    fovy -= yoffset / 30; // yoffset�� ���� ���� �ø� �� ���, �Ʒ��� ���� �� ����
    if (fovy < 0) fovy = 0;
    else if (fovy > PI) fovy = PI;
}

#endif
