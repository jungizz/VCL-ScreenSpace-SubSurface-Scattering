//
//  Created by Song Jungeun on 2024/1/4
//


#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "toys.h"
#include "objLoader.h"
#include "moveCam.h"

using namespace glm;

void render(GLFWwindow* window);
void init();


int main(void) 
{
    if (!glfwInit()) exit(EXIT_FAILURE);                                    // glfw 핵심 객체 초기화
    glfwWindowHint(GLFW_SAMPLES, 8);                                        // 생성할 Window의 기본 설정
    GLFWwindow* window = glfwCreateWindow(640, 480, "Hello", NULL, NULL);   // 창 객체 생성

    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetScrollCallback(window, scrollCallback);

    glfwMakeContextCurrent(window);            // 생성된 창에 대응되는 opengl 컨텍스트 객체 생성        
    glewInit();
    init();                                    
    glfwSwapInterval(1);                       // 스왑 간격 : 0 설정하면 fps 제한X, 1 설정하면 fps 제한 60
    while (!glfwWindowShouldClose(window)) {   // 창이 닫히기 전까지 무한루프
        render(window);
        glfwSwapBuffers(window);
        glfwPollEvents();                      // 대기 중인 이벤트 처리
    }                                          
    glfwDestroyWindow(window);                 // 루프가 끝났으므로 종료
    glfwTerminate();
}



GLuint vertexBuffer = 0; // 버퍼 ID (GLuint: Opengl의 unsigned long형)
GLuint normalBuffer = 0;
GLuint texcoordBuffer = 0;
GLuint vertexArray = 0; // 버텍스어레이 ID
GLuint elementBuffer = 0;
Program program;

GLuint diffTex = 0; // duffuse map ID
GLuint normTex = 0; // normal map ID
GLuint roughTex = 0; // roughness map ID
GLuint specTex = 0; // specularAO map ID (빨간부분은 specular, 파란부분은 ambient occlusion? 요런느낌)

vec3 lightPosition = vec3(3, 10, 5);
vec3 lightColor = vec3(500);
vec3 ambientLight = vec3(0.0);


void init() {
    if (!loadObj("LPS_Head.obj")) {
        std::cerr << "Failed to load the model!" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    program.loadShaders("shader.vert", "shader.frag");


    // <버텍스 정보 저장>
    // 1. Vertex Buffer Object (VBO)
    glGenBuffers(1, &vertexBuffer); // 버퍼 1개 생성
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer); // 사용할 버퍼 선언(바인딩; 하이라이팅)
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(vec3), vertices.data(), GL_STATIC_DRAW); // 버퍼에 버텍스 정보 데이터 저장

    glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), normals.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &texcoordBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, texcoordBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec2), texcoords.data(), GL_STATIC_DRAW);

    // 2. Vertex Array Object (VAO)
    glGenVertexArrays(1, &vertexArray); // 버텍스어레이 1개 생성
    glBindVertexArray(vertexArray); // 사용할 버텍스어레이 선언

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glEnableVertexAttribArray(0); // 버텍스어레이 안에 버텍스 속성을 담을 0번 공간 활성화
    glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0); // 활성화한 속성을 가진 버퍼를 0번 공간에 저장 (버텍스 당 숫자 3개 x, y, z)

    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, 0, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, texcoordBuffer);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, 0, 0, 0);


    glGenBuffers(1, &elementBuffer); 
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer); 
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles.size() * sizeof(u32vec3), triangles.data(), GL_STATIC_DRAW);

    // Texture
    // load diffuse map
    int w, h, n;
    void* buf = stbi_load("LPS_lambertian.jpg", &w, &h, &n, 4);
    glGenTextures(1, &diffTex);
    glBindTexture(GL_TEXTURE_2D, diffTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, buf);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, buf);
    stbi_image_free(buf);

    // load normal map
    buf = stbi_load("LPS_NormalMap.png", &w, &h, &n, 4);
    glGenTextures(1, &normTex);
    glBindTexture(GL_TEXTURE_2D, normTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, buf);
    stbi_image_free(buf);

    // load roughness map
    buf = stbi_load("LPS_Roughness.png", &w, &h, &n, 4);
    glGenTextures(1, &roughTex);
    glBindTexture(GL_TEXTURE_2D, roughTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, buf);
    stbi_image_free(buf);

    // load specularAO map
    buf = stbi_load("LPS+SpecularAO.png", &w, &h, &n, 4);
    glGenTextures(1, &specTex);
    glBindTexture(GL_TEXTURE_2D, specTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, buf);
    stbi_image_free(buf);


}


void render(GLFWwindow* window) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glClearColor(0.1, 0.1, 0.1, 0);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);


    glUseProgram(program.programID);

    vec3 initialCameraPosition = vec3(0, 0, cameraDistance);         // 초기 카메라 위치
    mat4 cameraRotationMatrix1 = rotate(cameraPhi, vec3(1, 0, 0));   // 수평 회전 행렬
    mat4 cameraRotationMatrix2 = rotate(cameraTheta, vec3(0, 1, 0)); // 수직 회전 행렬

    vec3 cameraPosition = cameraRotationMatrix2 * cameraRotationMatrix1 * vec4(initialCameraPosition, 1);

    mat4 viewMat = glm::lookAt(cameraPosition, vec3(0, 0, 0), vec3(0, 1, 0));    // 뷰행렬
    mat4 projMat = glm::perspective(fovy, width / (float)height, 0.01f, 1000.f); // 투영행렬

    GLuint modelMatLocation = glGetUniformLocation(program.programID, "modelMat");
    glUniformMatrix4fv(modelMatLocation, 1, 0, value_ptr(mat4(1)));

    GLuint viewMatLocation = glGetUniformLocation(program.programID, "viewMat");
    glUniformMatrix4fv(viewMatLocation, 1, 0, value_ptr(viewMat));

    GLuint projMatLocation = glGetUniformLocation(program.programID, "projMat");
    glUniformMatrix4fv(projMatLocation, 1, 0, value_ptr(projMat));

    GLuint cameraPositionLocation = glGetUniformLocation(program.programID, "cameraPosition");
    glUniform3fv(cameraPositionLocation, 1, value_ptr(cameraPosition));

    /*GLuint colorLocation = glGetUniformLocation(program.programID, "color");
    glUniform4fv(colorLocation, 1, value_ptr(diffuseColor[0]));

    GLuint shininessLocation = glGetUniformLocation(program.programID, "shininess");
    glUniform1f(shininessLocation, shininess[0]);*/

    GLuint lightPositionLocation = glGetUniformLocation(program.programID, "lightPosition");
    glUniform3fv(lightPositionLocation, 1, value_ptr(lightPosition));

    GLuint lightColorLocation = glGetUniformLocation(program.programID, "lightColor");
    glUniform3fv(lightColorLocation, 1, value_ptr(lightColor));

    GLuint ambientLightLocation = glGetUniformLocation(program.programID, "ambientLight");
    glUniform3fv(ambientLightLocation, 1, value_ptr(ambientLight));



    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffTex);
    GLuint diffTexLocation = glGetUniformLocation(program.programID, "diffTex");
    glUniform1i(diffTexLocation, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normTex);
    GLuint normTexLocation = glGetUniformLocation(program.programID, "normTex");
    glUniform1i(normTexLocation, 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, roughTex);
    GLuint roughTexLocation = glGetUniformLocation(program.programID, "roughTex");
    glUniform1i(roughTexLocation, 2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, specTex);
    GLuint specTexLocation = glGetUniformLocation(program.programID, "specTex");
    glUniform1i(specTexLocation, 3);


    glBindVertexArray(vertexArray);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glDrawElements(GL_TRIANGLES, triangles.size()*3, GL_UNSIGNED_INT, 0);
}

