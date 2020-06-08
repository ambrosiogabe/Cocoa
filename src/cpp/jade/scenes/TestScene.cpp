#include "jade/scenes/TestScene.h"
#include "jade/components/components.h"
#include "jade/platform/windows/GlFunctions.h"
#include "jade/renderer/Shader.h"
#include "jade/util/Log.h"

#include <windows.h>
#include <gl/GL.h>
#include <gl/glext.h>
#include <entt/entt.h>
#include <stdio.h>

typedef unsigned int uint;

float vertices[] = {
    -0.5f, -0.5f, 0.0f,       1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, 0.0f,       0.0f, 1.0f, 1.0f,
     0.0f,  0.5f, 0.0f,       0.0f, 1.0f, 0.0f
};

uint VAO;
Shader* shader = nullptr;

void TestScene::Init() {
    OutputDebugStringA("Initializing test scene.\n");
}

void TestScene::Start() {
    OutputDebugStringA("Starting test scene.\n");

    // ENTT test 1
    // entt::entity entity = m_Registry.create();
    // m_Registry.emplace<Transform>(entity, glm::vec3(10, 10, 10), glm::vec3(0, 90, 0), glm::vec3(1, 1, 1));

    shader = new Shader("C:\\dev\\c++\\dungeonCrawler\\assets\\shaders\\default.glsl");

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    uint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}


void TestScene::Render() {
    //OutputDebugStringA("Rendering test scene.\n");
    Log::Info("Rendreing test scene.");
    shader->Bind();
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    shader->Unbind();
}

void TestScene::Update(float dt) {
    char str[512];
    // snprintf(str, sizeof(str), "Updating test scene at: %2.3fms\n", dt);
    // OutputDebugStringA(str);
    Log::Info("Updating test scene at: %2.3fms", dt);

    // ENTT test 1
    // auto view = m_Registry.view<Transform>();

    // for (auto entity : view) {
    //     auto &transform = group.get<Transform>(entity);

    //     snprintf(str, sizeof(str), "Position: %2.2f, %2.2f, %2.2f\nSize: %2.2f, %2.2f, %2.2f\nRotation: %2.2f %2.2f %2.2f\n\n",
    //         transform.m_Position.x, transform.m_Position.y, transform.m_Position.z, transform.m_Scale.x, transform.m_Scale.y, transform.m_Scale.z, 
    //         transform.m_EulerRotation.x, transform.m_EulerRotation.y, transform.m_EulerRotation.z);
    //     OutputDebugStringA(str);
    // }
}