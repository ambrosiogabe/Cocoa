#include "jade/scenes/TestScene.h"
#include "jade/components/components.h"
#include "jade/platform/windows/GlFunctions.h"
#include "jade/renderer/Shader.h"
#include "jade/util/Log.h"
#include "jade/events/Input.h"
#include "jade/Jade.h"

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

    float startX = 100.0f;
    float startY = 80.0f;
    float width = 20.0f;
    float height = 20.0f;
    float padding = 3.0f;
    for (int i=0; i < 100; i++) {
        for (int j=0; j < 80; j++) {
            float x = startX + i * width + i * padding;
            float y = startY + j * height + j * padding;

            float r = x / (100 * width);
            float g = y / (80 * height);
            float b = 1.0f;

            entt::actor a1 = entt::actor(m_Registry);
            a1.assign<Transform>(glm::vec3(x, y, 0), glm::vec3(width, height, 1.0f), glm::vec3(0, 0, 0));
            a1.assign<SpriteRenderer>(glm::vec4(r, g, b, 1));
            m_RenderSystem->AddActor(a1);
        }
    }
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

    const GLubyte* vendor = glGetString(GL_VENDOR); // Returns the vendor
    const GLubyte* renderer = glGetString(GL_RENDERER); // Returns a hint to the model
    Log::Info("Renderer: %s", renderer);
    Log::Info("Vendor: %s", vendor);
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
    float speed = 500.0f;
    if (Input::KeyPressed(JADE_KEY_W)) {
        m_Camera->GetTransform().m_Position.y += dt * speed;
    } else if (Input::KeyPressed(JADE_KEY_S)) {
        m_Camera->GetTransform().m_Position.y -= dt * speed;
    }

    if (Input::KeyPressed(JADE_KEY_D)) {
        m_Camera->GetTransform().m_Position.x += dt * speed;
    } else if (Input::KeyPressed(JADE_KEY_A)) {
        m_Camera->GetTransform().m_Position.x -= dt * speed;
    }

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