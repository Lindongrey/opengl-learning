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
// 使用 target 而非 +z 单位向量（front）会在移动时导致一系列问题
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

    // 移动摄像机
    float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        // 可以看到 cameraTarget 为 vec3(0.0f, 0.0f, 0.0f) 的问题：
        // 当移动到 (0, 0, 0) ，也就是 cameraPos 的 z 值为 0 时，就会停下（在调试时是进入了立方体内部）
        // 将 cameraTarget 改为一个 -z 方向的单位向量，就可以无限前进
        // cameraPos -= cameraPos * cameraSpeed;
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        // cameraPos += cameraPos * cameraSpeed;
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        // 旋转而非平移
        // cameraPos -= glm::normalize(glm::cross((cameraPos - cameraTarget), cameraUp)) * cameraSpeed;
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        // cameraPos += glm::normalize(glm::cross((cameraPos - cameraTarget), cameraUp)) * cameraSpeed;
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    // 补偿不同渲染能力造成的速度差异
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
    // 初始化 GLFW
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

    // 隐藏鼠标
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // 设置鼠标转动
    glfwSetCursorPosCallback(window, mouse_callback);
    // 设置鼠标缩放
    glfwSetScrollCallback(window, scroll_callback);

    // glViewport(0, 0, 800, 800);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 加载 glad，否则无法使用 opengl 函数
    // 必须写在后面，因为要等 opengl 上下文先创建好
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
    // 以下开始是核心代码
    // 链接顶点属性
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
    // 十个箱子的位置
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
    // 创建 VBO VAO
    unsigned int VBO, cubeVAO, lightVAO;
    // 生成 VBO
    glGenBuffers(1, &VBO);
    // 把 VBO 绑定到 GL_ARRAY_BUFFER 目标上
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // 给 VBO 分配内存和数据
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 生成 VAO
    glGenVertexArrays(1, &cubeVAO);
    // 绑定 VAO，开始记录状态
    glBindVertexArray(cubeVAO);
    // 告诉 VAO 如何解析这些顶点
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    // 启用顶点属性位置 0，VAO 亦记录了这一条
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    // 解绑 VAO
    glBindVertexArray(0);

    // 生成 VAO
    glGenVertexArrays(1, &lightVAO);
    // 绑定 VAO，开始记录状态
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // 告诉 VAO 如何解析这些顶点
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    // 启用顶点属性位置 0，VAO 亦记录了这一条
    glEnableVertexAttribArray(0);
    // 解绑 VAO
    glBindVertexArray(0);

    // 解绑 VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // 加载图片
    int width, height, nrChannels;
    unsigned char* data = stbi_load("assets/container2.png", &width, &height, &nrChannels, 0);
    // 创建纹理
    unsigned int texture;
    glGenTextures(1, &texture);
    // 绑定纹理
    glBindTexture(GL_TEXTURE_2D, texture);
    // 设置两个方向的环绕
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    // 设置线性过滤
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 生成纹理
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    // 释放
    stbi_image_free(data);
    // 设置 uniform 变量，设置一次即可所以不用放在循环里，设置必须先启动着色器程序
    glUseProgram(ourShader.ID);
    glUniform1i(glGetUniformLocation(ourShader.ID, "material.diffuse"), 0);

    // 镜面反射贴图
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data2 = stbi_load("assets/container2_specular.png", &width, &height, &nrChannels, 0);
    // 创建纹理
    unsigned int texture2;
    glGenTextures(1, &texture2);
    // 绑定纹理
    glBindTexture(GL_TEXTURE_2D, texture2);
    // 设置两个方向的环绕
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    // 设置线性过滤
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 生成纹理
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data2);
    glGenerateMipmap(GL_TEXTURE_2D);
    // 释放
    stbi_image_free(data2);
    // 设置 uniform 变量，设置一次即可所以不用放在循环里，设置必须先启动着色器程序
    glUseProgram(ourShader.ID);
    glUniform1i(glGetUniformLocation(ourShader.ID, "material.specular"), 1);
     
    // 启用深度测试
    glEnable(GL_DEPTH_TEST);

    // 渲染循环
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        // 背景色，不是三角形的颜色，三角形的颜色在片段着色器中设置
        // glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // 清除颜色缓冲
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);        // 深度测试与清除上一帧的深度缓冲

        // 激活纹理
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        // 激活着色器程序
        ourShader.use();
        // 设置光源位置
        glUniform3fv(glGetUniformLocation(ourShader.ID, "light.position"), 1, glm::value_ptr(lightPos));
        // glUniform3f(glGetUniformLocation(ourShader.ID, "light.direction"), -0.2f, -1.0f, -0.3f); // 这是平行光的全局光照
        // 设置摄像头位置
        glUniform3fv(glGetUniformLocation(ourShader.ID, "cameraPos"), 1, glm::value_ptr(cameraPos));
        // 设置材质
        glUniform3f(glGetUniformLocation(ourShader.ID, "material.ambient"), 1.0f, 0.5f, 0.31f);
        // 本例中漫反射是正常的，只是与光源夹角太大，所以几乎看不见
        glUniform3f(glGetUniformLocation(ourShader.ID, "material.diffuse"), 1.0f, 0.5f, 0.31f);
        glUniform3f(glGetUniformLocation(ourShader.ID, "material.specular"), 0.5f, 0.5f, 0.5f);
        glUniform1f(glGetUniformLocation(ourShader.ID, "material.shininess"), 32.0f);
        // 设置光的颜色
        glUniform3f(glGetUniformLocation(ourShader.ID, "light.ambient"), 0.2f, 0.2f, 0.2f);
        glUniform3f(glGetUniformLocation(ourShader.ID, "light.diffuse"), 0.5f, 0.5f, 0.5f);
        glUniform3f(glGetUniformLocation(ourShader.ID, "light.specular"), 1.0f, 1.0f, 1.0f);
        // 设置光照衰减的三系数
        glUniform1f(glGetUniformLocation(ourShader.ID, "light.constant"), 1.0f);
        glUniform1f(glGetUniformLocation(ourShader.ID, "light.linear"), 0.09f);
        glUniform1f(glGetUniformLocation(ourShader.ID, "light.quadratic"), 0.032f);
        // 设置聚光
        glUniform1f(glGetUniformLocation(ourShader.ID, "light.cutOff"), glm::cos(glm::radians(45.0f)));
        glUniform1f(glGetUniformLocation(ourShader.ID, "light.outerCutOff"), glm::cos(glm::radians(55.0f)));
        glUniform3f(glGetUniformLocation(ourShader.ID, "light.direction"), 0.0f, 0.4f, 3.0f);
        // 将手电筒置于摄像机位置
        // glUniform3fv(glGetUniformLocation(ourShader.ID, "light.position"), 1, glm::value_ptr(cameraPos));
        // glUniform3fv(glGetUniformLocation(ourShader.ID, "light.direction"), 1, glm::value_ptr(cameraFront));
        // 设置物体
        // 绑定 VAO
        glBindVertexArray(cubeVAO);
        // 变换到世界空间
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
        // 观察空间
        glm::mat4 cubeView = glm::mat4(1.0f);
        cubeView = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(cubeView));
        // 设置透视投影矩阵
        glm::mat4 cubeProjection = glm::mat4(1.0f);
        cubeProjection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);
        glUniformMatrix4fv(glGetUniformLocation(ourShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(cubeProjection));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // 设置光源
        lightShader.use();
        // 绑定 VAO
        glBindVertexArray(lightVAO);
        // 变换到世界空间
        glm::mat4 lightModel;
        lightModel = glm::translate(lightModel, glm::vec3(lightPos));
        lightModel = glm::scale(lightModel, glm::vec3(0.1f, 0.1f, 0.1f));
        lightShader.setMat4("model", lightModel);
        // 观察空间
        glm::mat4 lightView = glm::mat4(1.0f);
        lightView = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(lightView));
        // 设置透视投影矩阵
        glm::mat4 lightProjection = glm::mat4(1.0f);
        lightProjection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);
        glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(lightProjection));
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        // 绘制矩形
        // 配置如何绘制，以下是线框模式
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 释放资源
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);

    // 释放窗口
    glfwTerminate();

    return 0;
}