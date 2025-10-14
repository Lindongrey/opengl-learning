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

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
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

    float sensitive = 0.1f;
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

    window = glfwCreateWindow(800, 600, "Learn Camera", NULL, NULL);

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

    Shader ourShader("../src/shaders/shader.vs", "../src/shaders/shader.fs");
    // ���¿�ʼ�Ǻ��Ĵ���
    // ���Ӷ�������
    // ����һ��������ɫ��ͬ��������
    // float vertices[] = {
    //     ---- λ�� ----      - �������� -
    //     0.5f,  0.5f, 0.0f,   1.0f, 1.0f,   // ����
    //     0.5f, -0.5f, 0.0f,   1.0f, 0.0f,   // ����
    //     -0.5f, -0.5f, 0.0f,   0.0f, 0.0f,   // ����
    //     -0.5f,  0.5f, 0.0f,   0.0f, 1.0f    // ����
    // };
    float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    // ��������
    unsigned int indices[] = {
        // ע��������0��ʼ! 
        // ����������(0,1,2,3)���Ƕ�������vertices���±꣬
        // �����������±��������ϳɾ���

        0, 1, 3, // ��һ��������
        1, 2, 3  // �ڶ���������
    };
    // ����һ�� VBO VAO EBO
    unsigned int VBO, VAO, EBO;
    // ���� VBO VAO EBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // �� VAO����ʼ��¼״̬
    glBindVertexArray(VAO);

    // �� VBO �󶨵� GL_ARRAY_BUFFER Ŀ����
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // �� VBO �����ڴ������
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // �� EBO �����ƵĲ���
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // ���� VAO ��ν�����Щ����
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    // ��ɫ����Ҫ
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FLOAT, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    // ���ö�������λ�� 0��VAO ���¼����һ��
    glEnableVertexAttribArray(0);
    // ����λ�� 1
    // glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    // ��� VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // ��� VAO
    glBindVertexArray(0);

    // ����ͼƬ
    int width, height, nrChannels;
    unsigned char* data = stbi_load("../assets/container.jpg", &width, &height, &nrChannels, 0);
    // ��������
    unsigned int texture, texture2;
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    // �ͷ�
    stbi_image_free(data);
    // ����ڶ�������
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data2 = stbi_load("../assets/awesomeface.png", &width, &height, &nrChannels, 0);
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // ���� png ����͸���ȣ�������Ҫʹ�� GL_RGBA ���� GL_RGB
    // ���ʹ���˺��ߣ��Ὣ RGBA ��ʽ��4 Bytes����ȡΪ RGB ��ʽ��3 Bytes��
    // Ҳ����˵����һ�����ص�͸������Ϣ�ᱻ��Ϊ��ɫ��Ϣ���Դ˵���
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data2);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data2);

    // ���� uniform ����������һ�μ������Բ��÷���ѭ������ñ�����������ɫ������
    ourShader.use();
    // ����� 0 �ͺ��� 231 �е� GL_TEXTURE0 ��Ӧ
    glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0);
    glUniform1i(glGetUniformLocation(ourShader.ID, "texture2"), 1);

    // ���� 10 ���������λ��
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

    // ��Ⱦѭ��
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        // ����ɫ�����������ε���ɫ�������ε���ɫ��Ƭ����ɫ��������
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // �����ɫ����
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        // ������ɫ������
        ourShader.use();
        // �� VAO
        glBindVertexArray(VAO);

        // ���� 10 ��������
        for (unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model;
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            // ȫ����ת
            // model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
            // ʹ�� shader ���еķ���
            ourShader.setMat4("model", model);
            // glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // �任������ռ��У�����������ƽ���ڵ���
        // ����Ŀǰ��û��͸�ӣ�����ֻ�Ǳ���
        // glm::mat4 model = glm::mat4(1.0f);
        // model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        // model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        // ��ʱ��任
        // model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        // ���ù۲����Ŀǰ������ʲô����
        // ���û��ͶӰ������ô��ʲô��û�У���Ϊ�����е���
        glm::mat4 view = glm::mat4(1.0f);
        // pos �� target ����ó� z ����Ȼ�� z �� up ��˵ó�һ������ͨ�����ҷ��� x��
        // ��� x �� z ��˵ó� y
        // view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        // view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        // glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
        
        // ����͸��ͶӰ����
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);
        glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        
        // ��Ȳ����������һ֡����Ȼ���
        glEnable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);
        
        // ���ƾ���
        // ������λ��ƣ��������߿�ģʽ
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // �ͷ���Դ
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // �ͷŴ���
    glfwTerminate();

    return 0;
}