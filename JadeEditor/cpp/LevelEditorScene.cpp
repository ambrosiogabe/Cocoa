#pragma once

#include "LevelEditorScene.h"
#include "LevelEditorSystem.h"
#include "Gizmos.h"
#include "Gui/ImGuiExtended.h"

#include "jade/systems/System.h"
#include "jade/renderer/DebugDraw.h"
#include "jade/physics2d/Physics2DSystem.h"
#include "jade/util/JMath.h"
#include "jade/util/Settings.h"
#include "jade/file/JPath.h"
#include "jade/core/AssetManager.h"
#include "FontAwesome.h"
#include "jade/core/Audio.h"
#include "jade/systems/RenderSystem.h"

#include <entt/entt.hpp>
#include <imgui.h>
#include <array>

namespace Jade
{
	void LevelEditorScene::Init()
	{
		LoadDefaultAssets();

		glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0);
		m_Camera = new Camera(cameraPos);

		Physics2D::Init(this);

		m_Systems.emplace_back(std::make_unique<RenderSystem>("Render System", this));
		m_Systems.emplace_back(std::make_unique<LevelEditorSystem>("LevelEditor System", this));
		m_Systems.emplace_back(std::make_unique<Physics2DSystem>("Physics2D System", this));
		m_Systems.emplace_back(std::make_unique<GizmoSystem>("Gizmo System", this));
	}

	void LevelEditorScene::Start()
	{
		for (const auto& system : m_Systems)
		{
			system->Start();
		}
	}

	void LevelEditorScene::Render()
	{
		for (const auto& system : m_Systems)
		{
			system->Render();
		}
	}

	void LevelEditorScene::Update(float dt)
	{
		if (m_IsRunning)
		{
			Physics2D::Get()->Update(dt);
		}

		for (const auto& system : m_Systems)
		{
			system->Update(dt);
		}
	}

	//void LevelEditorScene::ImGui()
	//{
	//	if (m_ShowDemoWindow)
	//	{
	//		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
	//		ImGui::ShowDemoWindow(&m_ShowDemoWindow);
	//	}

	//	ImGuiSceneHeirarchy();

	//	ImGui::Begin(ICON_FA_CUBE " Inspector");
	//	if (!m_ActiveEntity.IsNull())
	//	{
	//		int itemPressed = 0;
	//		std::array<const char*, 4> components = { "Sprite Renderer", "Rigidbody", "BoxCollider2D", "CircleCollider2D" };
	//		if (JImGui::ButtonDropdown(ICON_FA_PLUS " Add Component", components.data(), (int)components.size(), itemPressed))
	//		{
	//			switch (itemPressed)
	//			{
	//			case 0:
	//				m_ActiveEntity.AddComponent<SpriteRenderer>();
	//				break;
	//			case 1:
	//				m_ActiveEntity.AddComponent<Rigidbody2D>();
	//				break;
	//			case 2:
	//				m_ActiveEntity.AddComponent<Box2D>();
	//				break;
	//			case 4:
	//				m_ActiveEntity.AddComponent<Circle>();
	//				break;
	//			}
	//		}
	//	}
	//	else if (m_ActiveAsset)
	//	{
	//		std::shared_ptr<Texture> activeTexture = m_ActiveAsset->GetType() == Asset::GetResourceTypeId<Texture>() ?
	//			std::static_pointer_cast<Texture>(m_ActiveAsset) : nullptr;
	//		if (activeTexture)
	//		{
	//			Texture::ImGuiInspector(activeTexture);
	//		}
	//	}

	//	ImGui::End();
	//}
}