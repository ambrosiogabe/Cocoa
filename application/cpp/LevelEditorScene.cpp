#pragma once

#include "Jade.h"
#include "LevelEditorSystem.h"

namespace Jade {
    class LevelEditorScene : public Scene {
    public:
        virtual void Init() override {
            m_ImGuiLayer.Setup(Application::Get()->GetWindow()->GetNativeWindow());

            auto group = m_Registry.group<SpriteRenderer>(entt::get<Transform>);

            Log::Info("Initializing test scene.");

            m_Systems.emplace_back(std::make_unique<LevelEditorSystem>("LE System"));

            glm::vec3 cameraPos = glm::vec3(1920.0f/2.0f, 1080.0f/2.0f, 0);
            m_Camera = new Camera(cameraPos);
            m_Systems.emplace_back(std::make_unique<RenderSystem>(m_Camera, "Render System"));

            texture = new Texture("C:/dev/C++/DungeonCrawler/assets/images/decorationsAndBlocks.png");
            sprites = new Spritesheet(texture, 16, 16, 81, 0);

            entt::entity testEntity = m_Registry.create();
            m_Registry.emplace<Transform>(testEntity, glm::vec3(-100, 0, 0), glm::vec3(64, 64, 1), glm::vec3(0, 0, 0), "Textured Block");
            m_Registry.emplace<SpriteRenderer>(testEntity, glm::vec4(1, 1, 1, 1), sprites->GetSprite(80));
            m_ActiveEntity = testEntity;

            entt::entity container = m_Registry.create();
            m_Registry.emplace<Transform>(container, glm::vec3(), glm::vec3(1), glm::vec3(), "Container");

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
                    m_Registry.emplace<Transform>(e1, glm::vec3(x, y, 0), glm::vec3(width, height, 1.0f), glm::vec3(0, 0, 0), "Gen Block", container, previous);
                    m_Registry.emplace<SpriteRenderer>(e1, glm::vec4(r, g, b, 1));
                    previous = e1;
                }
            }
        }

        virtual void Start() override {
            shader = new Shader("C:/dev/C++/DungeonCrawler/assets/shaders/SpriteRenderer.glsl");

            for (const auto& system : m_Systems) {
                system->Start(m_Registry);
            }
        }

        virtual void Render() override {
            for (const auto& system : m_Systems) {
                system->Render(m_Registry);
            }
        }

        virtual void Update(float dt) override {
            // static float speed = 500.0f;
            // if (Input::KeyPressed(JADE_KEY_W)) {
            //     m_Camera->GetTransform().m_Position.y += dt * speed;
            // } else if (Input::KeyPressed(JADE_KEY_S)) {
            //     m_Camera->GetTransform().m_Position.y -= dt * speed;
            // }

            // if (Input::KeyPressed(JADE_KEY_D)) {
            //     m_Camera->GetTransform().m_Position.x += dt * speed;
            // } else if (Input::KeyPressed(JADE_KEY_A)) {
            //     m_Camera->GetTransform().m_Position.x -= dt * speed;
            // }

            // TODO: Zoom is broken, hopefully I'll fix it after I switch up my event system
            // if (Input::ScrollY() != 0) {
            //     speed = 500.0f * m_Camera->GetZoom();
            //     m_Camera->SetZoom(m_Camera->GetZoom() + ((float)Input::ScrollY() * 0.1f));
            // }

            for (const auto& system : m_Systems) {
                system->Update(m_Registry, dt);
            }
        }
        
        virtual void ImGui() override {
            m_ImGuiLayer.StartFrame();
            m_ImGuiLayer.ImGui();

            if (showDemoWindow) {
                ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
                ImGui::ShowDemoWindow(&showDemoWindow);
            }

            ImGui::Begin("Inspector");

            for (const auto& system : m_Systems) {
                system->ImGui(m_Registry);
                ImGui::Separator();
            }

            ImGui::End();

            m_ImGuiLayer.Render();
        }

    private:
        Shader* shader = nullptr;
        Spritesheet* sprites = nullptr;
        Texture* texture = nullptr;

        bool showDemoWindow = true;
    };
}