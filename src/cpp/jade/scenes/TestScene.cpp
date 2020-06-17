#include "jade/scenes/TestScene.h"
#include "jade/components/components.h"
#include "jade/platform/windows/GlFunctions.h"
#include "jade/renderer/Shader.h"
#include "jade/util/Log.h"
#include "jade/events/Input.h"
#include "jade/core/Jade.h"
#include "jade/systems/LevelEditorSystem.h"
#include "jade/platform/JWindow.h"

#include <windows.h>
#include <gl/GL.h>
#include <gl/glext.h>
#include <entt/entt.h>
#include <stdio.h>
#include <imgui/imgui.h>
#include <string>

typedef unsigned int uint;

Shader* shader = nullptr;
Spritesheet* sprites = nullptr;
Texture* texture = nullptr;

void TestScene::Init() {
    m_ImGuiLayer.Setup(JWindow::GetWindowHandle());

    auto group = m_Registry.group<SpriteRenderer>(entt::get<Transform>);

    for (int i=0; i < 100; i++) {
        m_Systems[i] = nullptr;
    }

    Log::Info("Initializing test scene.");

    LevelEditorSystem* levelEditorSystem = new LevelEditorSystem();
    AddSystem(levelEditorSystem);

    glm::vec3 cameraPos = glm::vec3(1920.0f/2.0f, 1080.0f/2.0f, 0);
    m_Camera = new Camera(cameraPos);
    RenderSystem* renderSystem = new RenderSystem(m_Camera);
    AddSystem(renderSystem);

    texture = new Texture("C:/dev/C++/DungeonCrawler/assets/images/decorationsAndBlocks.png");
    sprites = new Spritesheet(texture, 16, 16, 81, 0);

    entt::entity testEntity = m_Registry.create();
    m_Registry.assign<Transform>(testEntity, glm::vec3(-100, 0, 0), glm::vec3(64, 64, 1), glm::vec3(0, 0, 0), "Textured Block");
    m_Registry.assign<SpriteRenderer>(testEntity, glm::vec4(1, 1, 1, 1), sprites->GetSprite(80));
    m_ActiveEntity = testEntity;

    entt::entity container = m_Registry.create();
    m_Registry.assign<Transform>(container, glm::vec3(), glm::vec3(1), glm::vec3(), "Container");

    float startX = 100.0f;
    float startY = 80.0f;
    float width = 20.0f;
    float height = 20.0f;
    float padding = 3.0f;
    entt::entity previous = entt::null;
    for (int i=0; i < 100; i++) {
        for (int j=0; j < 100; j++) {
            float x = startX + i * width + i * padding;
            float y = startY + j * height + j * padding;

            float r = x / (100 * width);
            float g = y / (80 * height);
            float b = 1.0f;

            entt::entity e1 = m_Registry.create();
            m_Registry.assign<Transform>(e1, glm::vec3(x, y, 0), glm::vec3(width, height, 1.0f), glm::vec3(0, 0, 0), "Gen Block", container, previous);
            m_Registry.assign<SpriteRenderer>(e1, glm::vec4(r, g, b, 1));
            previous = e1;
        }
    }
}

void TestScene::Start() {
    shader = new Shader("C:/dev/C++/DungeonCrawler/assets/shaders/SpriteRenderer.glsl");

    for (int i=0; i < m_SystemCount; i++) {
        m_Systems[i]->Start(m_Registry);
    }
}


void TestScene::Render() {
    for (int i=0; i < m_SystemCount; i++) {
        m_Systems[i]->Render(m_Registry);
    }
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

    for (int i=0; i < m_SystemCount; i++) {
        m_Systems[i]->Update(m_Registry, dt);
    }
}

static bool showDemoWindow = true;
void TestScene::ImGui() {
    m_ImGuiLayer.StartFrame();
    m_ImGuiLayer.ImGui();

    if (showDemoWindow) {
        ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
        ImGui::ShowDemoWindow(&showDemoWindow);
    }

    ImGui::Begin("Inspector");

    for (int i=0; i < m_SystemCount; i++) {
        m_Systems[i]->ImGui(m_Registry);
        ImGui::Separator();
    }

    ImGui::End();

    m_ImGuiLayer.Render();
}