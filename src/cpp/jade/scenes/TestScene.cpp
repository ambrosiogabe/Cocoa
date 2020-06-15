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
    auto group = m_Registry.group<SpriteRenderer>(entt::get<Transform>);

    for (int i=0; i < 100; i++) {
        m_Systems[i] = nullptr;
    }

    Log::Info("Initializing test scene.");
    glm::vec3 cameraPos = glm::vec3(0, 0, 0);
    m_Camera = new Camera(cameraPos);
    m_RenderSystem = new RenderSystem(m_Camera, m_Registry);
    m_Systems[0] = static_cast<System*>(m_RenderSystem);

    texture = new Texture("C:/dev/C++/DungeonCrawler/assets/images/decorationsAndBlocks.png");
    sprites = new Spritesheet(texture, 16, 16, 81, 0);

    entt::entity testEntity = m_Registry.create();
    m_Registry.assign<Transform>(testEntity, glm::vec3(-100, 0, 0), glm::vec3(64, 64, 1), glm::vec3(0, 0, 0));
    m_Registry.assign<SpriteRenderer>(testEntity, glm::vec4(1, 1, 1, 1), sprites->GetSprite(80));
    m_ActiveEntity = &testEntity;

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
        }
    }
}

void TestScene::Start() {
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
}

void TestScene::ImGui() {
    for (int i=0; i < 100; i++) {
        if (m_Systems[i] != nullptr) {
            m_Systems[i]->ImGui();
        } else {
            break;
        }
    }
}