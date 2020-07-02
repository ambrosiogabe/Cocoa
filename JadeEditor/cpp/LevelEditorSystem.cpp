#include "LevelEditorSystem.h"
#include "jade/core/Application.h"
#include "jade/events/Input.h"
#include "jade/commands/CommandHistory.h"
#include "jade/commands/MoveTransformCommand.h"
#include "jade/renderer/DebugDraw.h"

#include <string>
#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace Jade {
    void LevelEditorSystem::Start(entt::registry& registry) {
        auto view = registry.view<Transform>();
        for (auto entity : view) {
            Transform& transform = view.get<Transform>(entity);
            if (entt::to_integral(transform.m_Previous) != entt::to_integral(entt::null)) {
                Transform& prevTransform = view.get<Transform>(transform.m_Previous);
                prevTransform.m_Next = entity;

                if (entt::to_integral(transform.m_Parent) != entt::to_integral(entt::null)) {
                    Transform& parentTransform = view.get<Transform>(transform.m_Parent);
                    if (entt::to_integral(transform.m_First) == entt::to_integral(entt::null)) {
                        parentTransform.m_First = entity;
                    }
                }
            }
        }
    }

    void LevelEditorSystem::Update(entt::registry& registry, float dt) {
        Application::Get()->GetScene()->GetCamera()->GetTransform().m_Position += m_CameraSpeed * dt;

        // Set active entity to mouse pos if dragging
        if (m_IsDragging) {
            Transform& transform = registry.get<Transform>(Application::Get()->GetScene()->GetActiveEntity());
            glm::vec3 newPos = glm::vec3(Input::OrthoMouseX() - m_DragOffset.x, Input::OrthoMouseY() - m_DragOffset.y, 0.0f);

            CommandHistory::AddCommand(new MoveTransformCommand(transform, newPos));
        }

        // Draw grid lines
        Transform& cameraTransform = Application::Get()->GetScene()->GetCamera()->GetTransform();
        float cameraZoom = Application::Get()->GetScene()->GetCamera()->GetZoom();
        int gridWidth = Constants::GridSizeX * cameraZoom;
        int gridHeight = Constants::GridSizeY * cameraZoom;

        float firstX = (float)(((int)(cameraTransform.m_Position.x - cameraZoom*1920.0f/2.0f) / gridWidth) - 1) * (float)gridWidth;
        float firstY = (float)(((int)(cameraTransform.m_Position.y - cameraZoom*1080.0f/2.0f) / gridHeight) - 1) * (float)gridHeight;

        int yLinesNeeded = (int)((cameraZoom*1920 + gridWidth) / gridWidth);
        int xLinesNeeded = (int)((cameraZoom*1080 + gridHeight) / gridHeight);

        for (int i=0; i < yLinesNeeded; i++) {
            float x = (i * gridWidth) + firstX;
            float y = (i * gridHeight) + firstY;
            glm::vec2 from(x, firstY - gridHeight);
            glm::vec2 to(x, firstY + 1080 + gridWidth);
            glm::vec3 color(0.2f, 0.2f, 0.2f);
            DebugDraw::AddLine2D(from, to, 1.0f, color);

            if (i <= xLinesNeeded) {
                glm::vec2 from2(firstX - gridWidth, y);
                glm::vec2 to2(firstX + 1920 + gridHeight, y);
                glm::vec3 color2(0.2f, 0.2f, 0.2f);
                DebugDraw::AddLine2D(from2, to2, 1.0f, color2);
            }
        }
    }

    void LevelEditorSystem::ImGui(entt::registry& registry) {
        ImGui::Begin("Scene");
        int index = 0;
        auto view = registry.view<Transform>();
        
        for (auto entity : view) {
            Transform& transform = view.get<Transform>(entity);
            if (entt::to_integral(transform.m_Parent) == entt::to_integral(entt::null)) {
                DoTreeNode(index, transform, registry);
            }
        }
        ImGui::End();

        entt::entity activeEntity = Application::Get()->GetScene()->GetActiveEntity();
        if (registry.has<Transform>(activeEntity)) {
            Transform& transform = registry.get<Transform>(activeEntity);
            if (ImGui::CollapsingHeader("Transform")) {
                ImGui::DragFloat3("Position: ", glm::value_ptr(transform.m_Position));
                ImGui::DragFloat3("Scale: ", glm::value_ptr(transform.m_Scale));
                ImGui::DragFloat3("Rotation: ", glm::value_ptr(transform.m_EulerRotation));
            }
        }
    }


    void LevelEditorSystem::DoTreeNode(int& index, const Transform& transform, const entt::registry& registry) {
        std::string str = transform.m_Name;
        std::string res = str + "##" + std::to_string(index);
        index++;

        entt::entity next = transform.m_Next;
        while (entt::to_integral(next) != entt::to_integral(entt::null)) {
            const Transform& transform2 = registry.get<Transform>(next);
            std::string str2 = transform2.m_Name;
            std::string res2 = str2 + "##" + std::to_string(index);
            index++;

            if (ImGui::TreeNode(res2.c_str())) {
                if (entt::to_integral(transform2.m_First) != entt::to_integral(entt::null)) {
                    const Transform& childTrans = registry.get<Transform>(transform2.m_First);
                    DoTreeNode(index, childTrans, registry);
                }
                ImGui::TreePop();
            }
            next = transform2.m_Next;
        }

        if(ImGui::TreeNode(res.c_str())) {
            if (entt::to_integral(transform.m_First) != entt::to_integral(entt::null)) {
                const Transform& firstTransform = registry.get<Transform>(transform.m_First);
                DoTreeNode(index, firstTransform, registry);
            }
            ImGui::TreePop();
        }
    }

    void LevelEditorSystem::OnEvent(Event& e) {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<KeyPressedEvent>(std::bind(&LevelEditorSystem::HandleKeyPress, this, std::placeholders::_1));
        dispatcher.Dispatch<KeyReleasedEvent>(std::bind(&LevelEditorSystem::HandleKeyRelease, this, std::placeholders::_1));
        dispatcher.Dispatch<MouseButtonPressedEvent>(std::bind(&LevelEditorSystem::HandleMouseButtonPressed, this, std::placeholders::_1));
        dispatcher.Dispatch<MouseButtonReleasedEvent>(std::bind(&LevelEditorSystem::HandleMouseButtonReleased, this, std::placeholders::_1));
        dispatcher.Dispatch<MouseScrolledEvent>(std::bind(&LevelEditorSystem::HandleMouseScroll, this, std::placeholders::_1));
    }

    bool LevelEditorSystem::HandleKeyPress(KeyPressedEvent& e) {
        if (e.GetKeyCode() == JADE_KEY_LEFT_CONTROL) {
            m_ControlModifierPressed = true;
        }

        static float speed = 500.0f;
        if (e.GetKeyCode() == JADE_KEY_W) {
            m_CameraSpeed.y = speed;
        } else if (e.GetKeyCode() == JADE_KEY_S) {
            m_CameraSpeed.y = -speed;
        }

        if (e.GetKeyCode() == JADE_KEY_A) {
            m_CameraSpeed.x = -speed;
        } else if (e.GetKeyCode() == JADE_KEY_D) {
            m_CameraSpeed.x = speed;
        }

        if (e.GetKeyCode() == JADE_KEY_Z && m_ControlModifierPressed) {
            CommandHistory::Undo();
        }

        if (e.GetKeyCode() == JADE_KEY_R && m_ControlModifierPressed) {
            CommandHistory::Redo();
        }

        return false;
    }

    bool LevelEditorSystem::HandleKeyRelease(KeyReleasedEvent& e) {
        if (e.GetKeyCode() == JADE_KEY_LEFT_CONTROL) {
            m_ControlModifierPressed = false;
        }

        if (e.GetKeyCode() == JADE_KEY_W) {
            m_CameraSpeed.y = 0;
        } else if (e.GetKeyCode() == JADE_KEY_S) {
            m_CameraSpeed.y = 0;
        }

        if (e.GetKeyCode() == JADE_KEY_A) {
            m_CameraSpeed.x = 0;
        } else if (e.GetKeyCode() == JADE_KEY_D) {
            m_CameraSpeed.x = 0;
        }

        return false;
    }

    bool LevelEditorSystem::HandleMouseScroll(MouseScrolledEvent& e) {
        float yOffset = -e.GetYOffset();
        if (yOffset != 0) {
            Camera* camera = Application::Get()->GetScene()->GetCamera();
            //float speed = 500.0f * camera->GetZoom();
            camera->SetZoom(camera->GetZoom() + (yOffset * 0.1f));
        }

        return false;
    }

    bool LevelEditorSystem::HandleMouseButtonPressed(MouseButtonPressedEvent& e) {
        // if (!m_IsDragging && e.GetMouseButton() == JADE_MOUSE_BUTTON_LEFT) {
        //     float worldX = Input::OrthoMouseX();
        //     float worldY = Input::OrthoMouseY();

        //     auto view = registry.view<Transform>();
        //     for (auto entity : view) {
        //         Transform& transform = view.get<Transform>(entity);
        //         if (worldX >= transform.m_Position.x && worldX <= transform.m_Position.x + transform.m_Scale.x && 
        //             worldY >= transform.m_Position.y && worldY <= transform.m_Position.y + transform.m_Scale.y) {
        //             Application::Get()->GetScene()->SetActiveEntity(entity);
        //             m_DebounceLeft = m_DebounceTime;

        //             m_IsDragging = true;
        //             m_DragOffset.x = Input::OrthoMouseX() - transform.m_Position.x;
        //             m_DragOffset.y = Input::OrthoMouseY() - transform.m_Position.y;
        //             break;
        //         }
        //     }
        // }

        return false;
    }

    bool LevelEditorSystem::HandleMouseButtonReleased(MouseButtonReleasedEvent& e) {
        if (m_IsDragging && e.GetMouseButton() == JADE_MOUSE_BUTTON_LEFT) {
            CommandHistory::SetNoMergeMostRecent();
            m_IsDragging = false;
        }

        return false;
    }
}