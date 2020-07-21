#include "LevelEditorSystem.h"
#include "jade/core/Application.h"
#include "jade/events/Input.h"
#include "jade/commands/CommandHistory.h"
#include "jade/commands/ChangeVec3Command.h"
#include "jade/renderer/DebugDraw.h"
#include "jade/core/ImGuiExtended.h"
#include "jade/util/JMath.h"

#include <string>
#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace Jade
{
	void LevelEditorSystem::Start(entt::registry& registry)
	{
		auto view = registry.view<Transform>();
		for (auto entity : view)
		{
			Transform& transform = view.get<Transform>(entity);
			if (entt::to_integral(transform.m_Previous) != entt::to_integral(entt::null))
			{
				Transform& prevTransform = view.get<Transform>(transform.m_Previous);
				prevTransform.m_Next = entity;

				if (entt::to_integral(transform.m_Parent) != entt::to_integral(entt::null))
				{
					Transform& parentTransform = view.get<Transform>(transform.m_Parent);
					if (entt::to_integral(transform.m_First) == entt::to_integral(entt::null))
					{
						parentTransform.m_First = entity;
					}
				}
			}
		}
	}

	void LevelEditorSystem::Update(entt::registry& registry, float dt)
	{
		if (m_IsDragging)
		{
			Camera* camera = Application::Get()->GetScene()->GetCamera();
			glm::vec3 mousePosWorld = JMath::Vector3From2(camera->ScreenToOrtho());
			glm::vec3 delta = m_OriginalDragClickPos - mousePosWorld;
			delta *= 0.8f;
			camera->GetTransform().m_Position = m_OriginalCameraPos + delta;
		}

		// Draw grid lines
		if (Settings::General::s_DrawGrid)
		{
			Transform& cameraTransform = Application::Get()->GetScene()->GetCamera()->GetTransform();
			float cameraZoom = Application::Get()->GetScene()->GetCamera()->GetZoom();
			int gridWidth = Settings::General::s_GridSizeX;// *cameraZoom;
			int gridHeight = Settings::General::s_GridSizeY;// *cameraZoom;

			float firstX = (float)(((int)(cameraTransform.m_Position.x - cameraZoom * 1920.0f / 2.0f) / gridWidth) - 1) * (float)gridWidth;
			float firstY = (float)(((int)(cameraTransform.m_Position.y - cameraZoom * 1080.0f / 2.0f) / gridHeight) - 1) * (float)gridHeight;

			int yLinesNeeded = (int)((cameraZoom * 1920 + gridWidth) / gridWidth);
			int xLinesNeeded = (int)((cameraZoom * 1080 + gridHeight) / gridHeight);

			for (int i = 0; i < yLinesNeeded; i++)
			{
				float x = (i * gridWidth) + firstX + (gridWidth / 2.0f);
				float y = (i * gridHeight) + firstY + (gridHeight / 2.0f);
				glm::vec2 from(x, firstY - gridHeight);
				glm::vec2 to(x, firstY + 1080 + gridWidth);
				glm::vec3 color(0.2f, 0.2f, 0.2f);
				DebugDraw::AddLine2D(from, to, 1.0f, color);

				if (i <= xLinesNeeded)
				{
					glm::vec2 from2(firstX - gridWidth, y);
					glm::vec2 to2(firstX + 1920 + gridHeight, y);
					glm::vec3 color2(0.2f, 0.2f, 0.2f);
					DebugDraw::AddLine2D(from2, to2, 1.0f, color2, 1, false);
				}
			}
		}
	}

	void LevelEditorSystem::ImGui(entt::registry& registry)
	{
		entt::entity activeEntity = Application::Get()->GetScene()->GetActiveEntity();
		if (registry.has<Transform>(activeEntity))
		{
			Transform& transform = registry.get<Transform>(activeEntity);

			static bool collapsingHeaderOpen = true;
			ImGui::SetNextTreeNodeOpen(collapsingHeaderOpen);
			if (ImGui::CollapsingHeader("Transform"))
			{
				ImGui::UndoableDragFloat3("Position: ", transform.m_Position);
				ImGui::UndoableDragFloat3("Scale: ", transform.m_Scale);
				ImGui::UndoableDragFloat3("Rotation: ", transform.m_EulerRotation);
			}
		}
	}

	void LevelEditorSystem::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(std::bind(&LevelEditorSystem::HandleKeyPress, this, std::placeholders::_1));
		dispatcher.Dispatch<KeyReleasedEvent>(std::bind(&LevelEditorSystem::HandleKeyRelease, this, std::placeholders::_1));
		dispatcher.Dispatch<MouseButtonPressedEvent>(std::bind(&LevelEditorSystem::HandleMouseButtonPressed, this, std::placeholders::_1));
		dispatcher.Dispatch<MouseButtonReleasedEvent>(std::bind(&LevelEditorSystem::HandleMouseButtonReleased, this, std::placeholders::_1));
		dispatcher.Dispatch<MouseScrolledEvent>(std::bind(&LevelEditorSystem::HandleMouseScroll, this, std::placeholders::_1));
	}

	bool LevelEditorSystem::HandleKeyPress(KeyPressedEvent& e)
	{
		if (e.GetKeyCode() == JADE_KEY_LEFT_CONTROL)
		{
			m_ControlModifierPressed = true;
		}

		if (m_ControlModifierPressed)
		{
			if (e.GetKeyCode() == JADE_KEY_Z)
			{
				CommandHistory::Undo();
			}

			if (e.GetKeyCode() == JADE_KEY_R)
			{
				CommandHistory::Redo();
			}

			if (e.GetKeyCode() == JADE_KEY_S)
			{
				Application::Get()->GetScene()->Save(Settings::General::s_CurrentSaveFile);
			}
		}

		return false;
	}

	bool LevelEditorSystem::HandleKeyRelease(KeyReleasedEvent& e)
	{
		if (e.GetKeyCode() == JADE_KEY_LEFT_CONTROL)
		{
			m_ControlModifierPressed = false;
		}

		return false;
	}

	bool LevelEditorSystem::HandleMouseScroll(MouseScrolledEvent& e)
	{
		float yOffset = -e.GetYOffset();
		if (yOffset != 0)
		{
			Camera* camera = Application::Get()->GetScene()->GetCamera();
			//float speed = 500.0f * camera->GetZoom();
			camera->SetZoom(camera->GetZoom() + (yOffset * 0.1f));
		}

		return false;
	}

	bool LevelEditorSystem::HandleMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		static float speed = 500.0f;
		if (!m_IsDragging && e.GetMouseButton() == JADE_MOUSE_BUTTON_MIDDLE)
		{
			m_IsDragging = true;
			Camera* camera = Application::Get()->GetScene()->GetCamera();
			m_OriginalCameraPos = camera->GetTransform().m_Position;
			m_OriginalDragClickPos = JMath::Vector3From2(camera->ScreenToOrtho());
			//m_DragClickOffset = JMath::Vector3From2(camera->ScreenToOrtho()) - camera->GetTransform().m_Position;
		}

		return false;
	}

	bool LevelEditorSystem::HandleMouseButtonReleased(MouseButtonReleasedEvent& e)
	{
		if (m_IsDragging && e.GetMouseButton() == JADE_MOUSE_BUTTON_MIDDLE)
		{
			m_IsDragging = false;
		}
		return false;
	}
}