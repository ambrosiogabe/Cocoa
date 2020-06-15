#include "jade/scenes/TestScene.h"
#include "jade/components/components.h"
#include "jade/platform/windows/GlFunctions.h"
#include "jade/renderer/Shader.h"
#include "jade/util/Log.h"
#include "jade/events/Input.h"
#include "jade/core/Jade.h"

#include <windows.h>
#include <gl/GL.h>
#include <gl/glext.h>
#include <entt/entt.h>
#include <stdio.h>

typedef unsigned int uint;

Shader* shader = nullptr;
Spritesheet* sprites = nullptr;
Texture* texture = nullptr;

void TestScene::Init() {
    m_Registry = entt::registry();

    Log::Info("Initializing test scene.");
    glm::vec3 cameraPos = glm::vec3(0, 0, 0);
    m_Camera = new Camera(cameraPos);
    m_RenderSystem = new RenderSystem(m_Camera, m_Registry);

    texture = new Texture("C:/dev/C++/DungeonCrawler/assets/images/decorationsAndBlocks.png");
    sprites = new Spritesheet(texture, 16, 16, 81, 0);

    entt::entity testEntity = m_Registry.create();
    m_Registry.assign<Transform>(testEntity, glm::vec3(-100, 0, 0), glm::vec3(64, 64, 1), glm::vec3(0, 0, 0));
    m_Registry.assign<SpriteRenderer>(testEntity, glm::vec4(1, 1, 1, 1), sprites->GetSprite(80));
    m_RenderSystem->AddEntity(testEntity);

    float startX = 100.0f;
    float startY = 80.0f;
    float width = 20.0f;
    float height = 20.0f;
    float padding = 3.0f;
    for (int i=0; i < 100; i++) {
        for (int j=0; j < 100; j++) {
            float x = startX + i * width + i * padding;
            float y = startY + j * height + j * padding;

            float r = x / (100 * width);
            float g = y / (80 * height);
            float b = 1.0f;

            entt::entity e1 = m_Registry.create();
            m_Registry.assign<Transform>(e1, glm::vec3(x, y, 0), glm::vec3(width, height, 1.0f), glm::vec3(0, 0, 0));
            m_Registry.assign<SpriteRenderer>(e1, glm::vec4(r, g, b, 1));
            m_RenderSystem->AddEntity(e1);
        }
    }

    
    // auto view = m_Registry.view<Transform>();
    // for (auto entity : view) {
    //     Transform& component = view.get<Transform>(entity);
    //     Log::Info("Component address: %d", &component);
    // }
}

void TestScene::Start() {
    // ENTT test 1
    entt::entity entity = m_Registry.create();
    m_Registry.emplace<Transform>(entity, glm::vec3(10, 10, 10), glm::vec3(0, 90, 0), glm::vec3(1, 1, 1));

    shader = new Shader("C:/dev/C++/DungeonCrawler/assets/shaders/SpriteRenderer.glsl");
}


void TestScene::Render() {
    m_RenderSystem->Render();
}

void TestScene::Update(float dt) {
    static float speed = 500.0f;
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

    if (Input::ScrollY() != 0) {
        speed = 500.0f * m_Camera->GetZoom();
        m_Camera->SetZoom(m_Camera->GetZoom() + ((float)Input::ScrollY() * 0.1f));
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