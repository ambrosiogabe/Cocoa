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
    -0.5f, -0.5f, 0.0f,       1.0f, 0.0f, 0.0f, 1.0f,
     0.5f, -0.5f, 0.0f,       0.0f, 1.0f, 1.0f, 1.0f,
     0.0f,  0.5f, 0.0f,       0.0f, 1.0f, 0.0f, 1.0f
};

uint VAO;
Shader* shader = nullptr;

void TestScene::Init() {
    Log::Info("Initializing test scene.");
    m_Camera = new Camera(glm::vec3(0, 0, 0));
    m_RenderSystem = new RenderSystem(m_Camera);

    entt::actor a1 = entt::actor(m_Registry);
    a1.assign<Transform>(glm::vec3(1920.0f/2.0f, 0, 0), glm::vec3(1900.0f, 200.0f, 1.0f), glm::vec3(0, 0, 0));
    a1.assign<SpriteRenderer>(glm::vec4(1, 1, 0, 1));
    m_RenderSystem->AddActor(a1);
}

void TestScene::Start() {
    Log::Info("Starting test scene.");

    // ENTT test 1
    entt::entity entity = m_Registry.create();
    m_Registry.emplace<Transform>(entity, glm::vec3(10, 10, 10), glm::vec3(0, 90, 0), glm::vec3(1, 1, 1));

    shader = new Shader("C:/dev/C++/DungeonCrawler/assets/shaders/SpriteRenderer.glsl");

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    uint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}


void TestScene::Render() {
    m_RenderSystem->Render();

    // shader->Bind();
    // glBindVertexArray(VAO);
    // glEnableVertexAttribArray(0);
    // glEnableVertexAttribArray(1);

    // glDrawArrays(GL_TRIANGLES, 0, 3);
    
    // glDisableVertexAttribArray(0);
    // glDisableVertexAttribArray(1);
    // glBindVertexArray(0);
    // shader->Unbind();
}

void TestScene::Update(float dt) {
    m_Camera->GetTransform().m_Position.x -= dt * 200.0f;
    //Log::Info("Updating test scene at: %2.3fms", dt);

    // ENTT test 1
    // auto view = m_Registry.view<Transform>();

    // for (auto entity : view) {
    //     auto &transform = view.get<Transform>(entity);

    //     Log::Info("Position: %2.2f, %2.2f, %2.2f\nSize: %2.2f, %2.2f, %2.2f\nRotation: %2.2f %2.2f %2.2f\n",
    //         transform.m_Position.x, transform.m_Position.y, transform.m_Position.z, transform.m_Scale.x, transform.m_Scale.y, transform.m_Scale.z, 
    //         transform.m_EulerRotation.x, transform.m_EulerRotation.y, transform.m_EulerRotation.z);
    // }
}