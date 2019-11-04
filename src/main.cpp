#include <glad/glad.h>
#include <iostream>
#include <GLFW/glfw3.h>
#include <windows.h>
#include "stb_image.h"
#include "MyModel.h"
#include "Camera.h"

const int SCR_WIDTH = 1600;
const int SCR_HEIGHT = 1200;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
const unsigned int NUM_POINT_LIGHT = 3;

float vertices[] = {
    // positions       ,ds
    -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f,
    -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
    0.5f, 0.5f, 0.5f, 0.5f, 0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
    -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, -0.5f,
    -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, -0.5f};

void init();
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);

int main()
{
    init();
    // 创建窗口对象
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "fail" << std::endl;
        glfwTerminate();
        return -1;
    }

    // 将窗口的上下文设置为当前线程的主上下文
    glfwMakeContextCurrent(window);

    // 初始化glad，加载系统相关的opengl函数指针地址
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "fail load glad" << std::endl;
        return -1;
    }

    // 前两个参数控制窗口左下角的位置，后两个控制宽高
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    vector<glm::vec3> lightPos;
    float radius = 3;
    float perAngle = 360.0 / NUM_POINT_LIGHT;
    for (int i = 0; i < NUM_POINT_LIGHT; i++)
    {
        lightPos.push_back(glm::vec3(radius * sin(glm::radians(i * perAngle)), -1, radius * cos(glm::radians(i * perAngle))));
    }

    unsigned int lightVAO, lightVBO;
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &lightVBO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    Shader lightShader("../shaders/light.vs", "../shaders/light.fs");

    Shader ourShader("../shaders/shader.vs", "../shaders/shader.fs");
    ourShader.use();
    ourShader.setInt("material.shininess", 32);

    MyModel ourModel("../model/dog/test.obj");

    while (!glfwWindowShouldClose(window))
    {
        Sleep(5);

        processInput(window);
        // 渲染指令
        // 用于清屏的颜色
        glClearColor(0.5, 0.5, 0.5, 0.5);
        // 选择要清除的缓存类型，有颜色缓存，深度缓存等可选
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(camera.zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.getViewMatrix();
        ourShader.use();
        // glm::vec3 lightColor = glm::vec3(sin(glfwGetTime()) * cos(glfwGetTime()), cos(glfwGetTime()) * cos(glfwGetTime()), sin(glfwGetTime()));
        glm::vec3 lightColor = glm::vec3(1, 1, 1);
        for (int i = 0; i < NUM_POINT_LIGHT; i++)
        {
            stringstream ss;
            ss << i;
            string number = ss.str();
            ourShader.setVec3("pointLight[" + number + "].position", lightPos[i]);
            ourShader.setVec3("pointLight[" + number + "].ambient", glm::vec3(0.2) * lightColor);
            ourShader.setVec3("pointLight[" + number + "].diffuse", glm::vec3(0.8) * lightColor);
            ourShader.setVec3("pointLight[" + number + "].specular", lightColor);
            ourShader.setFloat("pointLight[" + number + "].constant", 1.0);
            ourShader.setFloat("pointLight[" + number + "].linear", 0.09);
            ourShader.setFloat("pointLight[" + number + "].quadratic", 0.032);
        }
        glm::mat4 model = glm::mat4(1.0f);
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        ourShader.setVec3("viewPos", camera.position);
        ourModel.draw(ourShader, glm::vec3(0.001));
        // ourModel.drawMeshByIndex(ourShader, 2, glm::vec3(0.01));

        glBindVertexArray(lightVAO);
        lightShader.use();
        lightShader.setMat4("projection", projection);
        lightShader.setMat4("view", view);
        lightShader.setVec3("lightColor", lightColor);
        for (int i = 0; i < NUM_POINT_LIGHT; i++)
        {
            model = glm::translate(glm::mat4(1), lightPos[i]);
            model = glm::scale(model, glm::vec3(0.1));
            lightShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // 交换颜色缓冲，即颜色会被绘制显示在屏幕上
        glfwSwapBuffers(window);
        // 检查是否触发回调，并调用相应回调
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void init()
{
    // 初始化glfw
    glfwInit();
    // 配置glfw，第一个参数代表选项名称，第二个参数表示值
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void processInput(GLFWwindow *window)
{
    Camera_Movement move;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        move = FORWARD;
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        move = BACKWARD;
    else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        move = LEFT;
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        move = RIGHT;
    camera.processKeyboard(move);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
        return;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    camera.processMouseMovement(xoffset, yoffset, 90);

    lastX = xpos;
    lastY = ypos;
}