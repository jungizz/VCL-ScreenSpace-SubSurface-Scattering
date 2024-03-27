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

// Framebuffer Object structure
struct FBO {
    GLuint frameBuffer = 0;
    GLuint colorTexBuffer = 0;
    GLuint depthBuffer = 0;
};

void render(GLFWwindow* window);
void init();

GLuint loadTextureMap(const char* filename);
void attachBuffers(FBO* fbo); 

vec2 windowSize = { 1080, 720 };

int main(void) 
{
    if (!glfwInit()) exit(EXIT_FAILURE);                                    // glfw 핵심 객체 초기화
    glfwWindowHint(GLFW_SAMPLES, 8);                                        // 생성할 Window의 기본 설정
    GLFWwindow* window = glfwCreateWindow(windowSize.x, windowSize.y, "Hello", NULL, NULL);   // 창 객체 생성

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

GLuint quadVertexBuffer = 0;
GLuint quadArrrayBuffer = 0;
float quadVertices[] = { // 화면 전체에 렌더링하기 위한 사각형 정점 데이터
    -1.0f,  1.0f, 0.0f,// 왼쪽 상단
    -1.0f, -1.0f, 0.0f,// 왼쪽 하단
     1.0f, -1.0f, 0.0f,// 오른쪽 하단

    -1.0f,  1.0f, 0.0f,// 왼쪽 상단
     1.0f, -1.0f, 0.0f,// 오른쪽 하단
     1.0f,  1.0f, 0.0f,// 오른쪽 상단
};


FBO diffFBO;
FBO specFBO;
FBO rowGaussianFBO;
FBO colGaussianFBO;

Program diffProgram;
Program specProgram;
Program screenProgram;
Program rowGaussianProgram;
Program colGaussianProgram;

GLuint diffTex = 0; // duffuse map ID
GLuint normTex = 0; // normal map ID
GLuint roughTex = 0; // roughness map ID
GLuint specAOTex = 0; // specularAO map ID (빨간부분은 specular, 파란부분은 ambient occlusion? 요런느낌)

vec3 lightPosition = vec3(3, 3, 10);
vec3 lightColor = vec3(500);
vec3 ambientLight = vec3(0.0);


void init() {
    if (!loadObj("resources/LPS_Head.obj")) {
        std::cerr << "Failed to load the model!" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    diffProgram.loadShaders("diffShader.vert", "diffShader.frag");
    specProgram.loadShaders("specShader.vert", "specShader.frag");
    screenProgram.loadShaders("screenShader.vert", "screenShader.frag");
    rowGaussianProgram.loadShaders("gaussianBlur.vert", "rowGaussianBlur.frag");
    colGaussianProgram.loadShaders("gaussianBlur.vert", "colGaussianBlur.frag");

    // Vertex Buffer Object (VBO)
    glGenBuffers(1, &vertexBuffer); // 버퍼 1개 생성
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer); // 사용할 버퍼 선언(바인딩; 하이라이팅)
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(vec3), vertices.data(), GL_STATIC_DRAW); // 버퍼에 버텍스 정보 데이터 저장

    glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), normals.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &texcoordBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, texcoordBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec2), texcoords.data(), GL_STATIC_DRAW);

    // Vertex Array Object (VAO)
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
    diffTex= loadTextureMap("resources/LPS_lambertian.jpg");
    normTex = loadTextureMap("resources/LPS_NormalMap.png");
    roughTex = loadTextureMap("resources/LPS_Roughness.png");
    specAOTex = loadTextureMap("resources/LPS_SpecularAO.png");


    // screen quad VAO
    glGenBuffers(1, &quadVertexBuffer);
    glGenVertexArrays(1, &quadArrrayBuffer);
    glBindVertexArray(quadArrrayBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, quadVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, 0);


    // diffuse Frame Buffer Object
    glGenFramebuffers(1, &diffFBO.frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, diffFBO.frameBuffer);
    attachBuffers(&diffFBO);

    // gaussian Frame Buffer Object 
    glGenFramebuffers(1, &rowGaussianFBO.frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, rowGaussianFBO.frameBuffer);
    attachBuffers(&rowGaussianFBO);

    glGenFramebuffers(1, &colGaussianFBO.frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, colGaussianFBO.frameBuffer);
    attachBuffers(&colGaussianFBO);
}


void render(GLFWwindow* window) 
{
    // 1. draw on diffuse FBO
    glBindFramebuffer(GL_FRAMEBUFFER, diffFBO.frameBuffer);
    ivec2 nowSize;
    glfwGetFramebufferSize(window, &nowSize.x, &nowSize.y);
    glViewport(0, 0, nowSize.x, nowSize.y);
    glClearColor(0.1, 0.1, 0.1, 0);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(diffProgram.programID);

    vec3 initialCameraPosition = vec3(0, 0, cameraDistance);         // 초기 카메라 위치
    mat4 cameraRotationMatrix1 = rotate(cameraPhi, vec3(1, 0, 0));   // 수평 회전 행렬
    mat4 cameraRotationMatrix2 = rotate(cameraTheta, vec3(0, 1, 0)); // 수직 회전 행렬
    vec3 cameraPosition = cameraRotationMatrix2 * cameraRotationMatrix1 * vec4(initialCameraPosition, 1);

    mat4 viewMat = glm::lookAt(cameraPosition, vec3(0, 0, 0), vec3(0, 1, 0));           // 뷰행렬
    mat4 projMat = glm::perspective(fovy, nowSize.x / (float)nowSize.y, 0.01f, 10.f); // 투영행렬

    GLuint modelMatLocation = glGetUniformLocation(diffProgram.programID, "modelMat");
    glUniformMatrix4fv(modelMatLocation, 1, 0, value_ptr(mat4(1)));

    GLuint viewMatLocation = glGetUniformLocation(diffProgram.programID, "viewMat");
    glUniformMatrix4fv(viewMatLocation, 1, 0, value_ptr(viewMat));

    GLuint projMatLocation = glGetUniformLocation(diffProgram.programID, "projMat");
    glUniformMatrix4fv(projMatLocation, 1, 0, value_ptr(projMat));

    GLuint cameraPositionLocation = glGetUniformLocation(diffProgram.programID, "cameraPosition");
    glUniform3fv(cameraPositionLocation, 1, value_ptr(cameraPosition));

    GLuint lightPositionLocation = glGetUniformLocation(diffProgram.programID, "lightPosition");
    glUniform3fv(lightPositionLocation, 1, value_ptr(lightPosition));

    GLuint lightColorLocation = glGetUniformLocation(diffProgram.programID, "lightColor");
    glUniform3fv(lightColorLocation, 1, value_ptr(lightColor));

    GLuint ambientLightLocation = glGetUniformLocation(diffProgram.programID, "ambientLight");
    glUniform3fv(ambientLightLocation, 1, value_ptr(ambientLight));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, normTex);
    GLuint normTexLocation = glGetUniformLocation(diffProgram.programID, "normTex");
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specAOTex);
    GLuint specTexLocation = glGetUniformLocation(diffProgram.programID, "specAOTex");
    glUniform1i(specTexLocation, 1);

    glBindVertexArray(vertexArray);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glDrawElements(GL_TRIANGLES, triangles.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);


    // 2. draw on rowGaussianFBO
    glBindFramebuffer(GL_FRAMEBUFFER, rowGaussianFBO.frameBuffer);
    glViewport(0, 0, nowSize.x, nowSize.y);
    glClearColor(0.1, 0.1, 0.1, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(rowGaussianProgram.programID);

    // diffFBO에 있는 텍스처 사용해서 가우시안 하기 위해 보내기
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffFBO.colorTexBuffer);
    GLuint colorTexLocation = glGetUniformLocation(rowGaussianProgram.programID, "colorTex");
    glUniform1i(colorTexLocation, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, diffFBO.depthBuffer);
    GLuint depthTexLocation = glGetUniformLocation(rowGaussianProgram.programID, "depthTex");
    glUniform1i(depthTexLocation, 1);

    GLuint sizeLocation = glGetUniformLocation(rowGaussianProgram.programID, "size");
    glUniform2f(sizeLocation, static_cast<float>(nowSize.x), static_cast<float>(nowSize.y));

    GLuint widthLocation = glGetUniformLocation(rowGaussianProgram.programID, "screenWidth");
    glUniform1f(widthLocation, 2 * 0.01 * (tan(fovy / 2) * (nowSize.x/nowSize.y))); // screen width in world coord

    // Draw a quad to apply Gaussian blur
    glBindVertexArray(quadArrrayBuffer);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    // 3. draw on colGaussianFBO 
    glBindFramebuffer(GL_FRAMEBUFFER, colGaussianFBO.frameBuffer);
    glViewport(0, 0, nowSize.x, nowSize.y);
    glClearColor(0.1, 0.1, 0.1, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(colGaussianProgram.programID);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, rowGaussianFBO.colorTexBuffer);
    colorTexLocation = glGetUniformLocation(colGaussianProgram.programID, "colorTex");
    glUniform1i(colorTexLocation, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, rowGaussianFBO.depthBuffer);
    depthTexLocation = glGetUniformLocation(colGaussianProgram.programID, "depthTex");
    glUniform1i(depthTexLocation, 1);

    sizeLocation = glGetUniformLocation(colGaussianProgram.programID, "size");
    glUniform2f(sizeLocation, static_cast<float>(nowSize.x), static_cast<float>(nowSize.y));

    GLuint heightLocation = glGetUniformLocation(colGaussianProgram.programID, "screenHeight");
    glUniform1f(heightLocation, 2 * 0.01 * tan(fovy / 2)); // screen height in world coord

    // Draw a quad to apply Gaussian blur
    glBindVertexArray(quadArrrayBuffer);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);


    // 4. draw on specular FBO
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, nowSize.x, nowSize.y);
    glClearColor(0.1, 0.1, 0.1, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(specProgram.programID);

    modelMatLocation = glGetUniformLocation(specProgram.programID, "modelMat");
    glUniformMatrix4fv(modelMatLocation, 1, 0, value_ptr(mat4(1)));

    viewMatLocation = glGetUniformLocation(specProgram.programID, "viewMat");
    glUniformMatrix4fv(viewMatLocation, 1, 0, value_ptr(viewMat));

    projMatLocation = glGetUniformLocation(specProgram.programID, "projMat");
    glUniformMatrix4fv(projMatLocation, 1, 0, value_ptr(projMat));

    cameraPositionLocation = glGetUniformLocation(specProgram.programID, "cameraPosition");
    glUniform3fv(cameraPositionLocation, 1, value_ptr(cameraPosition));

    lightPositionLocation = glGetUniformLocation(specProgram.programID, "lightPosition");
    glUniform3fv(lightPositionLocation, 1, value_ptr(lightPosition));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffTex);
    GLuint diffTexLocation = glGetUniformLocation(specProgram.programID, "diffTex");
    glUniform1i(diffTexLocation, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, roughTex);
    GLuint roughTexLocation = glGetUniformLocation(specProgram.programID, "roughTex");
    glUniform1i(roughTexLocation, 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, colGaussianFBO.colorTexBuffer);
    GLuint finDiffTexLocation = glGetUniformLocation(specProgram.programID, "gaussianDiffTex");
    glUniform1i(finDiffTexLocation, 2);

    sizeLocation = glGetUniformLocation(specProgram.programID, "size");
    glUniform2f(sizeLocation, static_cast<float>(nowSize.x), static_cast<float>(nowSize.y));

    glBindVertexArray(vertexArray);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    glDrawElements(GL_TRIANGLES, triangles.size() * 3, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

GLuint loadTextureMap(const char* filename)
{
    int w, h, n;
    GLuint texID;
    void* buf = stbi_load(filename, &w, &h, &n, 4);
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, buf);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(buf);
    return texID;
}

void attachBuffers(FBO* fbo) 
{
    // texture buffer object to be used for colorbuffer
    glGenTextures(1, &fbo->colorTexBuffer);
    glBindTexture(GL_TEXTURE_2D, fbo->colorTexBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, windowSize.x, windowSize.y, 0, GL_RGB, GL_FLOAT, NULL); // image data를 load하여 연결할 필요 없으므로 마지막 인자에 NULL
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo->colorTexBuffer, 0); // attachment

    // texture buffer object to be used for depth buffer
    glGenTextures(1, &fbo->depthBuffer);
    glBindTexture(GL_TEXTURE_2D, fbo->depthBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, windowSize.x, windowSize.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL); // image data를 load하여 연결할 필요 없으므로 마지막 인자에 NULL
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fbo->depthBuffer, 0); // attachment

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
}
