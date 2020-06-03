#include "jade/scenes/TestScene.h"
#include "jade/components/components.h"
#include "jade/platform/windows/GlFunctions.h"


#include <windows.h>
#include <gl/GL.h>
#include <gl/glext.h>
#include <entt/entt.h>
#include <stdio.h>

typedef unsigned int uint;

float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};

uint VAO;

const char* vertexSrc = "#version 330 core\n"
                        "layout (location = 0) in vec3 aPos;\n"
                        "void main()\n"
                        "{\n"
                            "gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                        "}\0";

const char* fragmentSrc = "#version 330 core\n"
                          "out vec4 FragColor;\n"
                          "void main()\n"
                          "{\n"
                              "FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                          "}";                        
uint shaderProgram;


void TestScene::Init() {
    OutputDebugStringA("Initializing test scene.\n");
}

void TestScene::Start() {
    OutputDebugStringA("Starting test scene.\n");

    // ENTT test 1
    // entt::entity entity = m_Registry.create();
    // m_Registry.emplace<Transform>(entity, glm::vec3(10, 10, 10), glm::vec3(0, 90, 0), glm::vec3(1, 1, 1));

    uint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSrc, NULL);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        OutputDebugStringA(infoLog);
    }

    uint fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSrc, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        OutputDebugStringA(infoLog);
    }

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        OutputDebugStringA(infoLog);
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    uint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
}

void TestScene::Render() {
    OutputDebugStringA("Rendering test scene.\n");
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void TestScene::Update(float dt) {
    char str[512];
    snprintf(str, sizeof(str), "Updating test scene at: %2.3fms\n", dt);
    OutputDebugStringA(str);

    // ENTT test 1
    // auto group = m_Registry.view<Transform>();

    // for (auto entity : group) {
    //     auto &transform = group.get<Transform>(entity);

    //     snprintf(str, sizeof(str), "Position: %2.2f, %2.2f, %2.2f\nSize: %2.2f, %2.2f, %2.2f\nRotation: %2.2f %2.2f %2.2f\n\n",
    //         transform.m_Position.x, transform.m_Position.y, transform.m_Position.z, transform.m_Scale.x, transform.m_Scale.y, transform.m_Scale.z, 
    //         transform.m_EulerRotation.x, transform.m_EulerRotation.y, transform.m_EulerRotation.z);
    //     OutputDebugStringA(str);
    // }
}