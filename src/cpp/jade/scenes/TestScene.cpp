#include "jade/scenes/TestScene.h"
#include "jade/components/components.h"

#include <entt/entt.h>
#include <stdio.h>
#include <windows.h>

void TestScene::Init() {
    OutputDebugStringA("Initializing test scene.\n");
}

void TestScene::Start() {
    OutputDebugStringA("Starting test scene.\n");

    // ENTT test 1
    // entt::entity entity = m_Registry.create();
    // m_Registry.emplace<Transform>(entity, glm::vec3(10, 10, 10), glm::vec3(0, 90, 0), glm::vec3(1, 1, 1));
}

void TestScene::Render() {
    OutputDebugStringA("Rendering test scene.\n");
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