#include "Gizmos.h"
#include "JadeEditorApplication.h"
#include "PickingTexture.h"
#include "EditorWindows/InspectorWindow.h"

#include "jade/physics2d/Physics2DSystem.h"
#include "jade/renderer/DebugDraw.h"
#include "jade/renderer/Camera.h"
#include "jade/core/Application.h"
#include "jade/core/Core.h"
#include "jade/util/JMath.h"
#include "jade/core/Entity.h"
#include "jade/commands/ICommand.h"

namespace Jade
{
	// =================================================================================================
	// Gizmo
	// =================================================================================================
	Gizmo::Gizmo(const Sprite& sprite, glm::vec3 offset, float spriteRotation, GizmoType type, glm::vec3 darkTint)
	{
		m_Active = false;
		m_Position = glm::vec3();
		m_Box2D = { glm::vec2(10, 40), glm::vec2(5, 20), glm::vec2(0, 0) };
		m_HalfSize = { 8, 20 };

		m_Offset = offset;
		m_SpriteRotation = spriteRotation;
		m_TexCoordMin = sprite.m_TexCoords[0];
		m_TexCoordMax = sprite.m_TexCoords[2];
		m_TextureAssetId = sprite.m_Texture->GetResourceId();
		m_Tint = darkTint;
		m_Type = type;
	}

	void Gizmo::Render(Camera* camera)
	{
		float cameraZoom = camera->GetZoom() * 2;
		if (m_Active)
		{
			DebugDraw::AddSprite(m_TextureAssetId, m_HalfSize * 2.0f * cameraZoom, JMath::Vector2From3(m_Position), m_Tint, m_TexCoordMin, m_TexCoordMax, m_SpriteRotation);
		}
		else
		{
			DebugDraw::AddSprite(m_TextureAssetId, m_HalfSize * 2.0f * cameraZoom, JMath::Vector2From3(m_Position), { 1.0f, 1.0f, 1.0f }, m_TexCoordMin, m_TexCoordMax, m_SpriteRotation);
		}
	}

	void Gizmo::GizmoManipulateTranslate(Transform& transform, const glm::vec3& originalDragClickPos, const glm::vec3& mouseOffset, Camera* camera)
	{
		glm::vec3 mousePosWorld = JMath::Vector3From2(camera->ScreenToOrtho());
		glm::vec3 startToMouse = mousePosWorld - originalDragClickPos;
		glm::vec3 newPos;

		if (m_Type == GizmoType::Vertical)
		{
			newPos = glm::vec3(0, startToMouse.y, 0) + originalDragClickPos - mouseOffset;
		}
		else if (m_Type == GizmoType::Horizontal)
		{
			newPos = glm::vec3(startToMouse.x, 0, 0) + originalDragClickPos - mouseOffset;
		}
		else if (m_Type == GizmoType::Free)
		{
			newPos = startToMouse + originalDragClickPos - mouseOffset;
		}

		CommandHistory::AddCommand(new ChangeVec3Command(transform.m_Position, newPos));
	}

	void Gizmo::GizmoManipulateRotate(Transform& transform, const glm::vec3& startPos, const glm::vec3& mouseOffset, Camera* camera)
	{

	}

	void Gizmo::GizmoManipulateScale(Transform& transform, const glm::vec3& originalDragClickPos, const glm::vec3& originalScale, Camera* camera)
	{
		glm::vec3 mousePosWorld = JMath::Vector3From2(camera->ScreenToOrtho());
		glm::vec3 startToMouse = mousePosWorld - originalDragClickPos;
		float dragSpeed = 0.2f;
		glm::vec3 delta = glm::vec3(dragSpeed) * startToMouse;

		if (m_Type == GizmoType::Vertical)
		{
			delta = glm::vec3(0, delta.y, 0);
		}
		else if (m_Type == GizmoType::Horizontal)
		{
			delta = glm::vec3(delta.x, 0, 0);
		}

		CommandHistory::AddCommand(new ChangeVec3Command(transform.m_Scale, originalScale + delta));
	}

	// =================================================================================================
	// Gizmo Controller
	// =================================================================================================
	GizmoSystem::GizmoSystem(const char* name, Scene* scene)
		: System(name, scene)
	{
		Log::Assert(scene != nullptr, "Cannot initialize gizmo system with null scene.");

		m_Texture = std::static_pointer_cast<Texture>(AssetManager::GetAsset("assets/images/gizmos.png"));
		m_Spritesheet = std::unique_ptr<Spritesheet>(new Spritesheet(m_Texture, 16, 40, 9, 0));

		m_Camera = m_Scene->GetCamera();

		float hzOffsetX = 15;
		float hzOffsetY = -10;
		float squareOffsetX = 5;
		float squareOffsetY = 15;
		float vtOffsetX = -8;
		float vtOffsetY = 12;
		m_HzMove = Gizmo(m_Spritesheet->GetSprite(1), { hzOffsetX, hzOffsetY, 0.0f}, -90.0f, GizmoType::Horizontal);
		m_VtMove = Gizmo(m_Spritesheet->GetSprite(4), { vtOffsetX, vtOffsetY, 0.0f }, 0.0f, GizmoType::Vertical);
		m_FreeMove = Gizmo(m_Spritesheet->GetSprite(0), { squareOffsetX, squareOffsetY, 0.0f }, 0.0f, GizmoType::Free);
		m_FreeMove.m_Box2D = { glm::vec2(16, 16), glm::vec2(8, 8), glm::vec2(0, -12) };

		m_HzScale = Gizmo(m_Spritesheet->GetSprite(2), { hzOffsetX, hzOffsetY, 0.0f }, -90.0f, GizmoType::Horizontal);
		m_VtScale = Gizmo(m_Spritesheet->GetSprite(5), { vtOffsetX, vtOffsetY, 0.0f }, 0.0f, GizmoType::Vertical);
		m_FreeScale = Gizmo(m_Spritesheet->GetSprite(0), { squareOffsetX, squareOffsetY, 0.0f }, 0.0f, GizmoType::Free);
	}

	//void GizmoSystem::ImGui()
	//{
	//	ImGui::Begin("Gizmo debug");
	//	ImGui::DragFloat2("Free Move Offset:", &m_FreeMove.m_Offset[0]);
	//	ImGui::DragFloat2("Free Move HalfSize: ", &m_FreeMove.m_HalfSize[0]);
	//	ImGui::DragFloat2("Free Move BBHalfSize: ", &m_FreeMove.m_Box2D.m_HalfSize[0]);

	//	ImGui::DragFloat2("HzMove Offset:", &m_HzMove.m_Offset[0]);
	//	ImGui::DragFloat2("HzMove HalfSize: ", &m_HzMove.m_HalfSize[0]);
	//	ImGui::DragFloat2("HzMove BBHalfSize: ", &m_HzMove.m_Box2D.m_HalfSize[0]);

	//	ImGui::DragFloat2("VtMove Offset:", &m_VtMove.m_Offset[0]);
	//	ImGui::DragFloat2("VtMove HalfSize: ", &m_VtMove.m_HalfSize[0]);
	//	ImGui::DragFloat2("VtMove BBHalfSize: ", &m_VtMove.m_Box2D.m_HalfSize[0]);
	//	ImGui::End();
	//}

	void GizmoSystem::Update(float dt)
	{
		Entity activeEntity = InspectorWindow::GetActiveEntity();
		if (!activeEntity.IsNull())
		{
			Transform& entityTransform = activeEntity.GetComponent<Transform>();

			if (m_MouseDragging && m_ActiveGizmo >= 0)
			{
				Log::Assert((m_ActiveGizmo >= 0 && m_ActiveGizmo < 6), "Active gizmo out of array bounds.");
				switch (m_Mode)
				{
					case GizmoMode::Translate:
						m_Gizmos[m_ActiveGizmo].GizmoManipulateTranslate(entityTransform, m_OriginalDragClickPos, m_MouseOffset, m_Camera);
						break;
					case GizmoMode::Rotate:
						m_Gizmos[m_ActiveGizmo].GizmoManipulateRotate(entityTransform, m_OriginalDragClickPos, m_MouseOffset, m_Camera);
						break;
					case GizmoMode::Scale:
						m_Gizmos[m_ActiveGizmo].GizmoManipulateScale(entityTransform, m_OriginalDragClickPos, m_OriginalScale, m_Camera);
						break;
				}
			}

			int start = 0;
			int end = 3;
			if (m_Mode == GizmoMode::Translate)
			{
				start = 3;
				end = 6;
			}

			glm::vec2 mousePosWorld = m_Camera->ScreenToOrtho();
			bool anyHot = false;
			for (int i = start; i < end; i++)
			{
				Gizmo& gizmo = m_Gizmos[i];
				float cameraZoom = m_Camera->GetZoom() * 2;
				gizmo.m_Position = entityTransform.m_Position + gizmo.m_Offset * cameraZoom;
				glm::vec3 boxPos = gizmo.m_Position + JMath::Vector3From2(gizmo.m_Box2D.m_Offset) * cameraZoom;
				if (!m_MouseDragging && Physics2D::PointInBox(mousePosWorld, gizmo.m_Box2D.m_HalfSize * cameraZoom, boxPos, gizmo.m_SpriteRotation))
				{
					gizmo.m_Active = true;
					m_HotGizmo = i;
					anyHot = true;
				}
				else if (!m_MouseDragging && m_HotGizmo != i)
				{
					gizmo.m_Active = false;
				}
				gizmo.Render(m_Camera);
			}
			if (!anyHot)
			{
				m_HotGizmo = -1;
			}
		}
	}

	void GizmoSystem::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(std::bind(&GizmoSystem::HandleKeyPress, this, std::placeholders::_1));
		dispatcher.Dispatch<KeyReleasedEvent>(std::bind(&GizmoSystem::HandleKeyRelease, this, std::placeholders::_1));
		dispatcher.Dispatch<MouseButtonPressedEvent>(std::bind(&GizmoSystem::HandleMouseButtonPressed, this, std::placeholders::_1));
		dispatcher.Dispatch<MouseButtonReleasedEvent>(std::bind(&GizmoSystem::HandleMouseButtonReleased, this, std::placeholders::_1));
		dispatcher.Dispatch<MouseScrolledEvent>(std::bind(&GizmoSystem::HandleMouseScroll, this, std::placeholders::_1));
	}

	bool GizmoSystem::HandleKeyPress(KeyPressedEvent& e)
	{
		if (e.GetKeyCode() == JADE_KEY_S)
		{
			m_Mode = GizmoMode::Scale;
		} else if (e.GetKeyCode() == JADE_KEY_G)
		{
			m_Mode = GizmoMode::Translate;
		}
		return false;
	}

	bool GizmoSystem::HandleKeyRelease(KeyReleasedEvent& e)
	{

		return false;
	}

	bool GizmoSystem::HandleMouseScroll(MouseScrolledEvent& e)
	{

		return false;
	}

	bool GizmoSystem::HandleMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		if (!m_MouseDragging && e.GetMouseButton() == JADE_MOUSE_BUTTON_LEFT)
		{
			Camera* camera = m_Scene->GetCamera();
			glm::vec2 mousePosWorld = camera->ScreenToOrtho();

			glm::vec2 normalizedMousePos = Input::NormalizedMousePos();
			JadeEditor* editor = static_cast<JadeEditor*>(Application::Get());
			const PickingTexture& pickingTexture = editor->GetEditorLayer()->GetPickingTexture();
			PickingTexture::PixelInfo info = pickingTexture.ReadPixel((uint32)(normalizedMousePos.x * 3840), (uint32)(normalizedMousePos.y * 2160));

			Entity entity = m_Scene->GetEntity(info.m_EntityID);

			Entity selectedEntity = m_HotGizmo == -1 ? entity : InspectorWindow::GetActiveEntity();

			m_OriginalDragClickPos = JMath::Vector3From2(mousePosWorld);
			m_ActiveGizmo = -1;
			
			if (!selectedEntity.IsNull())
			{
				InspectorWindow::ClearAllEntities();
				InspectorWindow::AddEntity(selectedEntity);
				const Transform& transform = selectedEntity.GetComponent<Transform>();
				m_ActiveGizmo = m_HotGizmo;
				m_MouseDragging = true;
				m_MouseOffset = JMath::Vector3From2(mousePosWorld) - transform.m_Position;
				m_OriginalScale = transform.m_Scale;
			}
			else
			{
				InspectorWindow::ClearAllEntities();
				m_ActiveGizmo = -1;
			}
		}

		return false;
	}

	bool GizmoSystem::HandleMouseButtonReleased(MouseButtonReleasedEvent& e)
	{
		if (m_MouseDragging && e.GetMouseButton() == JADE_MOUSE_BUTTON_LEFT)
		{
			m_MouseDragging = false;
			CommandHistory::SetNoMergeMostRecent();
		}
		return false;
	}
}