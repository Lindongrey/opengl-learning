#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// ������ɫ��Դ��
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"layout (location = 2) in vec2 aTexCoord;\n"
"out vec3 vertexColor;\n"
"out vec2 TexCoord;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos, 1.0);\n"
"   vertexColor = aColor;\n"
"   TexCoord = aTexCoord;\n"
"}\0";

// Ƭ����ɫ��Դ��
const char* fragmentShaderSource = "#version 330 core\n"
"in vec3 vertexColor;\n"
"in vec2 TexCoord;\n"
"out vec4 FragColor;\n"
"uniform sampler2D ourTexture;\n"
"uniform sampler2D ourTexture2;\n"
"void main()\n"
"{\n"
// "    FragColor = vec4(vertexColor, 1.0f);\n"
"   FragColor = mix(texture(ourTexture, TexCoord), texture(ourTexture2, TexCoord), 0.2);\n"
"}\0";

int main() {
    // ��ʼ�� GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Hello Triangle", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // glViewport(0, 0, 800, 800);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // ���� glad�������޷�ʹ�� opengl ����
    // ����д�ں��棬��ΪҪ�� opengl �������ȴ�����
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // ���¿�ʼ�Ǻ��Ĵ���

    // ������ɫ��
    // ����һ��������ɫ��
    unsigned int vertexShader;
    // ������ɫ����ָ��Ϊ������ɫ��
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // ����ɫ��Դ����ӵ���ɫ����
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    // ������ɫ��
    glCompileShader(vertexShader);
    // ��鶥����ɫ�������Ƿ�ɹ�
    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Ƭ����ɫ��
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // ����һ����ɫ�����򣬲���������ɫ������󶨵�����
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    // �󶨵�������
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // ����Ƿ�ʧ��
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::ATTACHING_FAILED\n" << infoLog << std::endl;
    }
    // ������ɫ�������ڰ󶨺�Ϳ���ɾ��
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // ���Ӷ�������
    // ֮ǰһֱ�ڶ���ɫ�����г�ʼ��������Ҫ��ʼ����������
    // ���������ֻ꣨�����꣬û�������Ķ�����Ϣ
    float vertices[] = {
        //     ---- λ�� ----       ---- ��ɫ ----     - �������� -
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // ����
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // ����
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // ����
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // ����
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
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    // ������λ�ú���ɫ��������Ҫ���ν������ԣ����һ������Ϊ offset
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    // ��������������Ҫ������offset �� 6 ��ʼ
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    // ���ö�������λ�� 0��VAO ���¼����һ��
    glEnableVertexAttribArray(0);
    // ����λ�� 1
    glEnableVertexAttribArray(1);
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
    glUseProgram(shaderProgram);
    // ����� 0 �ͺ��� 231 �е� GL_TEXTURE0 ��Ӧ
    glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture2"), 1);

    // ��Ⱦѭ��
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        // ����ɫ�����������ε���ɫ�������ε���ɫ��Ƭ����ɫ��������
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // �����ɫ����
        glClear(GL_COLOR_BUFFER_BIT);

        // ������
        // glBindTexture(GL_TEXTURE_2D, texture);
        // ʹ������Ԫ��������
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        // ������ɫ������
        glUseProgram(shaderProgram);
        // �� VAO
        glBindVertexArray(VAO);

        // ʹ�� uniform
        // ����ʱ��ı���ɫ���Ӻڵ�����
        // float timeValue = glfwGetTime();
        // float greenValue = sin(timeValue) / 2.0f + 0.5f;
        // ��ȡ ourColor ��λ��
        // int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
        // ���� uniform
        // glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

        // ���ƾ���
        // ������λ��ƣ��������߿�ģʽ
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // ����������
        // glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // �ͷ���Դ
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // �ͷŴ���
    glfwTerminate();

    return 0;
}