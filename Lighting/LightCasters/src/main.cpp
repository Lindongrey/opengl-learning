#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <filesystem>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#include"shader_s.h"

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

    Shader ourShader("shaders/shader.vs", "shaders/shader.fs");
    Shader lightShader("shaders/light.vs", "shaders/light.fs");
    // ���¿�ʼ�Ǻ��Ĵ���
    // ���Ӷ�������
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };
    // ʮ�����ӵ�λ��
    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    // ���� VBO VAO
    unsigned int VBO, cubeVAO, lightVAO;
    // ���� VBO
    glGenBuffers(1, &VBO);
    // �� VBO �󶨵� GL_ARRAY_BUFFER Ŀ����
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // �� VBO �����ڴ������
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // ���� VAO
    glGenVertexArrays(1, &cubeVAO);
    // �� VAO����ʼ��¼״̬
    glBindVertexArray(cubeVAO);
    // ���� VAO ��ν�����Щ����
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    // ���ö�������λ�� 0��VAO ���¼����һ��
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    // ��� VAO
    glBindVertexArray(0);

    // ���� VAO
    glGenVertexArrays(1, &lightVAO);
    // �� VAO����ʼ��¼״̬
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // ���� VAO ��ν�����Щ����
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    // ���ö�������λ�� 0��VAO ���¼����һ��
    glEnableVertexAttribArray(0);
    // ��� VAO
    glBindVertexArray(0);

    // ��� VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // ����ͼƬ
    int width, height, nrChannels;
    unsigned char* data = stbi_load("assets/container2.png", &width, &height, &nrChannels, 0);
    // ��������
    unsigned int texture;
    glGenTextures(1, &texture);
    // ������
    glBindTexture(GL_TEXTURE_2D, texture);
    // ������������Ļ���
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    // �������Թ���
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // ��������
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    // �ͷ�
    stbi_image_free(data);
    // ���� uniform ����������һ�μ������Բ��÷���ѭ������ñ�����������ɫ������
    glUseProgram(ourShader.ID);
    glUniform1i(glGetUniformLocation(ourShader.ID, "material.diffuse"), 0);

    // ���淴����ͼ
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data2 = stbi_load("assets/container2_specular.png", &width, &height, &nrChannels, 0);
    // ��������
    unsigned int texture2;
    glGenTextures(1, &texture2);
    // ������
    glBindTexture(GL_TEXTURE_2D, texture2);
    // ������������Ļ���
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    // �������Թ���
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // ��������
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data2);
    glGenerateMipmap(GL_TEXTURE_2D);
    // �ͷ�
    stbi_image_free(data2);
    // ���� uniform ����������һ�μ������Բ��÷���ѭ������ñ�����������ɫ������
    glUseProgram(ourShader.ID);
    glUniform1i(glGetUniformLocation(ourShader.ID, "material.specular"), 1);
     
    // ������Ȳ���
    glEnable(GL_DEPTH_TEST);

    // ��Ⱦѭ��
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        // ����ɫ�����������ε���ɫ�������ε���ɫ��Ƭ����ɫ��������
        // glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // �����ɫ����
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);        // ��Ȳ����������һ֡����Ȼ���

        // ��������
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        // ������ɫ������
        ourShader.use();
        // ���ù�Դλ��
        glUniform3fv(glGetUniformLocation(ourShader.ID, "light.position"), 1, glm::value_ptr(lightPos));
        // glUniform3f(glGetUniformLocation(ourShader.ID, "light.direction"), -0.2f, -1.0f, -0.3f); // ����ƽ�й��ȫ�ֹ���
        // ��������ͷλ��
        glUniform3fv(glGetUniformLocation(ourShader.ID, "cameraPos"), 1, glm::value_ptr(cameraPos));
        // ���ò���
        glUniform3f(glGetUniformLocation(ourShader.ID, "material.ambient"), 1.0f, 0.5f, 0.31f);
        // �������������������ģ�ֻ�����Դ�н�̫�����Լ���������
        glUniform3f(glGetUniformLocation(ourShader.ID, "material.diffuse"), 1.0f, 0.5f, 0.31f);
        glUniform3f(glGetUniformLocation(ourShader.ID, "material.specular"), 0.5f, 0.5f, 0.5f);
        glUniform1f(glGetUniformLocation(ourShader.ID, "material.shininess"), 32.0f);
        // ���ù����ɫ
        glUniform3f(glGetUniformLocation(ourShader.ID, "light.ambient"), 0.2f, 0.2f, 0.2f);
        glUniform3f(glGetUniformLocation(ourShader.ID, "light.diffuse"), 0.5f, 0.5f, 0.5f);
        glUniform3f(glGetUniformLocation(ourShader.ID, "light.specular"), 1.0f, 1.0f, 1.0f);
        // ���ù���˥������ϵ��
        glUniform1f(glGetUniformLocation(ourShader.ID, "light.constant"), 1.0f);
        glUniform1f(glGetUniformLocation(ourShader.ID, "light.linear"), 0.09f);
        glUniform1f(glGetUniformLocation(ourShader.ID, "light.quadratic"), 0.032f);
        // ���þ۹�
        glUniform1f(glGetUniformLocation(ourShader.ID, "light.cutOff"), glm::cos(glm::radians(45.0f)));
        glUniform1f(glGetUniformLocation(ourShader.ID, "light.outerCutOff"), glm::cos(glm::radians(55.0f)));
        glUniform3f(glGetUniformLocation(ourShader.ID, "light.direction"), 0.0f, 0.4f, 3.0f);
        // ���ֵ�Ͳ���������λ��
        // glUniform3fv(glGetUniformLocation(ourShader.ID, "light.position"), 1, glm::value_ptr(cameraPos));
        // glUniform3fv(glGetUniformLocation(ourShader.ID, "light.direction"), 1, glm::value_ptr(cameraFront));
        // ��������
        // �� VAO
        glBindVertexArray(cubeVAO);
        // �任������ռ�
        /* glm::mat4 cubeModel;
        cubeModel = glm::translate(cubeModel, glm::vec3(0.0f, 0.0f, -5.0f));
        cubeModel = glm::rotate(cubeModel, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        ourShader.setMat4("model", cubeModel); */
        for (unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 cubeModel;
            cubeModel = glm::translate(cubeModel, cubePositions[i]);
            float angle = 20.0f * i;
            cubeModel = glm::rotate(cubeModel, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            ourShader.setMat4("model", cubeModel);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        // �۲�ռ�
        glm::mat4 cubeView = glm::mat4(1.0f);
        cubeView = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(cubeView));
        // ����͸��ͶӰ����
        glm::mat4 cubeProjection = glm::mat4(1.0f);
        cubeProjection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);
        glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(cubeProjection));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // ���ù�Դ
        lightShader.use();
        // �� VAO
        glBindVertexArray(lightVAO);
        // �任������ռ�
        glm::mat4 lightModel;
        lightModel = glm::translate(lightModel, glm::vec3(lightPos));
        lightModel = glm::scale(lightModel, glm::vec3(0.1f, 0.1f, 0.1f));
        lightShader.setMat4("model", lightModel);
        // �۲�ռ�
        glm::mat4 lightView = glm::mat4(1.0f);
        lightView = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(lightView));
        // ����͸��ͶӰ����
        glm::mat4 lightProjection = glm::mat4(1.0f);
        lightProjection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);
        glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(lightProjection));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        // ���ƾ���
        // ������λ��ƣ��������߿�ģʽ
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // �ͷ���Դ
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);

    // �ͷŴ���
    glfwTerminate();

    return 0;
}