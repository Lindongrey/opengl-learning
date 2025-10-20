#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <filesystem>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
#include <Model.h>
#include <filesystem>
#include <camera.h>
#include"shader_s.h"

namespace fs = std::filesystem;

glm::vec3 cameraPos = glm::vec3(0.0f, 1.5f, 3.0f);
// ʹ�� target ���� +z ��λ������front�������ƶ�ʱ����һϵ������
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float pitch = 0.0f;
float yaw = -90.0f;
float roll = 0.0f;
float lastX = 400.0f;
float lastY = 300.0f;
float fov = 45.0f;
bool firstMouse = true;
glm::vec3 lightPos = glm::vec3(0.0f, 1.2f, -3.0f);
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // �ƶ������
    float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        // ���Կ��� cameraTarget Ϊ vec3(0.0f, 0.0f, 0.0f) �����⣺
        // ���ƶ��� (0, 0, 0) ��Ҳ���� cameraPos �� z ֵΪ 0 ʱ���ͻ�ͣ�£��ڵ���ʱ�ǽ������������ڲ���
        // �� cameraTarget ��Ϊһ�� -z ����ĵ�λ�������Ϳ�������ǰ��
        // cameraPos -= cameraPos * cameraSpeed;
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        // cameraPos += cameraPos * cameraSpeed;
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        // ��ת����ƽ��
        // cameraPos -= glm::normalize(glm::cross((cameraPos - cameraTarget), cameraUp)) * cameraSpeed;
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        // cameraPos += glm::normalize(glm::cross((cameraPos - cameraTarget), cameraUp)) * cameraSpeed;
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    // ������ͬ��Ⱦ������ɵ��ٶȲ���
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}

void mouse_callback(GLFWwindow* window, double xPos, double yPos)
{
    if (firstMouse)
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    float xOffset = xPos - lastX;
    float yOffset = lastY - yPos;
    lastX = xPos;
    lastY = yPos;

    float sensitive = 0.01f;
    xOffset *= sensitive;
    yOffset *= sensitive;

    pitch += yOffset;
    yaw += xOffset;

    if (pitch > 89.0)
        pitch = 89.0;
    if (pitch < -89.0)
        pitch = -89.0;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
    fov -= (float)yOffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}

int initGLFW(GLFWwindow*& window) {
    // ��ʼ�� GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(800, 600, "Phone Lighting", NULL, NULL);

    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // �������
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // �������ת��
    glfwSetCursorPosCallback(window, mouse_callback);
    // �����������
    glfwSetScrollCallback(window, scroll_callback);

    // glViewport(0, 0, 800, 800);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // ���� glad�������޷�ʹ�� opengl ����
    // ����д�ں��棬��ΪҪ�� opengl �������ȴ�����
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
}

int main() {
    GLFWwindow* window = nullptr;
    initGLFW(window);

    stbi_set_flip_vertically_on_load(true);

    Shader ourShader("shaders/shader.vs", "shaders/shader.fs");

    char path[] = "resources/backpack.obj";
    Model ourModel(path);

    // ���¿�ʼ�Ǻ��Ĵ���

    // ������Ȳ���
    glEnable(GL_DEPTH_TEST);

    // ��Ⱦѭ��
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        // ����ɫ�����������ε���ɫ�������ε���ɫ��Ƭ����ɫ��������
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // �����ɫ����
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);        // ��Ȳ����������һ֡����Ȼ���

        // ������ɫ������
        ourShader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // �ͷŴ���
    glfwTerminate();

    return 0;
}