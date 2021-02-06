#include "renderer/Gizmos.h"
#include "core/CocoaEditorApplication.h"
#include "editorWindows/InspectorWindow.h"

#include "cocoa/physics2d/Physics2DSystem.h"
#include "cocoa/renderer/DebugDraw.h"
#include "cocoa/renderer/Camera.h"
#include "cocoa/core/Application.h"
#include "cocoa/core/Core.h"
#include "cocoa/util/CMath.h"
#include "cocoa/util/Settings.h"
#include "cocoa/core/Entity.h"
#include "cocoa/commands/ICommand.h"
#include "cocoa/core/AssetManager.h"

namespace Cocoa
{
	namespace Gizmo
	{
		// =================================================================================================
		// Gizmo
		// =================================================================================================
		GizmoData CreateGizmo(const Sprite& sprite, glm::vec3 offset, float spriteRotation, GizmoType type, glm::vec3 darkTint)
		{
			GizmoData data;
			data.Active = false;
			data.Position = glm::vec3();
			data.Box2D = { glm::vec2(10, 40), glm::vec2(5, 20), glm::vec2(0, 0) };
			data.HalfSize = { 8, 20 };

			data.Offset = offset;
			data.SpriteRotation = spriteRotation;
			data.TexCoordMin = sprite.m_TexCoords[0];
			data.TexCoordMax = sprite.m_TexCoords[2];
			data.TextureAssetId = sprite.m_Texture.m_AssetId;
			data.Tint = darkTint;
			data.Type = type;
			return data;
		}

		void Render(const GizmoData& data, Camera* camera)
		{
			float cameraZoom = camera->GetZoom() * 2;
			if (data.Active)
			{
				DebugDraw::AddSprite(data.TextureAssetId, data.HalfSize * 2.0f * cameraZoom, CMath::Vector2From3(data.Position), data.Tint, data.TexCoordMin, data.TexCoordMax, data.SpriteRotation);
			}
			else
			{
				DebugDraw::AddSprite(data.TextureAssetId, data.HalfSize * 2.0f * cameraZoom, CMath::Vector2From3(data.Position), { 1.0f, 1.0f, 1.0f }, data.TexCoordMin, data.TexCoordMax, data.SpriteRotation);
			}
		}

		void GizmoManipulateTranslate(const GizmoData& data, Transform& transform, const glm::vec3& originalDragClickPos, const glm::vec3& mouseOffset, Camera* camera)
		{
			glm::vec3 mousePosWorld = CMath::Vector3From2(camera->ScreenToOrtho());
			glm::vec3 startToMouse = mousePosWorld - originalDragClickPos;
			glm::vec3 newPos;

			if (data.Type == GizmoType::Vertical)
			{
				newPos = glm::vec3(0, startToMouse.y, 0) + originalDragClickPos - mouseOffset;
			}
			else if (data.Type == GizmoType::Horizontal)
			{
				newPos = glm::vec3(startToMouse.x, 0, 0) + originalDragClickPos - mouseOffset;
			}
			else if (data.Type == GizmoType::Free)
			{
				newPos = startToMouse + originalDragClickPos - mouseOffset;
			}

			CommandHistory::AddCommand(new ChangeVec3Command(transform.m_Position, newPos));
		}

		void GizmoManipulateRotate(const GizmoData& data, Transform& transform, const glm::vec3& startPos, const glm::vec3& mouseOffset, Camera* camera)
		{

		}

		void GizmoManipulateScale(const GizmoData& data, Transform& transform, const glm::vec3& originalDragClickPos, const glm::vec3& originalScale, Camera* camera)
		{
			glm::vec3 mousePosWorld = CMath::Vector3From2(camera->ScreenToOrtho());
			glm::vec3 startToMouse = mousePosWorld - originalDragClickPos;
			float dragSpeed = 0.2f;
			glm::vec3 delta = glm::vec3(dragSpeed) * startToMouse;

			if (data.Type == GizmoType::Vertical)
			{
				delta = glm::vec3(0, delta.y, 0);
			}
			else if (data.Type == GizmoType::Horizontal)
			{
				delta = glm::vec3(delta.x, 0, 0);
			}

			CommandHistory::AddCommand(new ChangeVec3Command(transform.m_Scale, originalScale + delta));
		}
	}

	// =================================================================================================
	// Gizmo System
	// =================================================================================================
	namespace GizmoSystem
	{
		// Internal Variables
		static Handle<Texture> m_GizmoTexture = {};
		static std::unique_ptr<Spritesheet> m_GizmoSpritesheet = nullptr;

		static bool m_MouseDragging = false;
		static int m_ActiveGizmo = -1;
		static int m_HotGizmo = -1;
		static GizmoMode m_Mode = GizmoMode::Translate;

		static glm::vec3 m_MouseOffset;
		static glm::vec3 m_OriginalScale;

		static glm::vec3 m_OriginalDragClickPos;
		static Camera* m_Camera;

		static GizmoData Gizmos[6];

		// Forward Declarations        
		static bool HandleKeyPress(KeyPressedEvent& e, Scene* scene);
		static bool HandleKeyRelease(KeyReleasedEvent& e, Scene* scene);
		static bool HandleMouseButtonPressed(MouseButtonPressedEvent& e, Scene* scene);
		static bool HandleMouseButtonReleased(MouseButtonReleasedEvent& e, Scene* scene);
		static bool HandleMouseScroll(MouseScrolledEvent& e, Scene* scene);

		void Start(Scene* scene)
		{
			Log::Assert(scene != nullptr, "Cannot initialize gizmo system with null scene.");

			m_GizmoTexture = AssetManager::GetTexture(Settings::General::s_EngineAssetsPath + "images/gizmos.png");
			m_GizmoSpritesheet = std::unique_ptr<Spritesheet>(new Spritesheet(m_GizmoTexture, 16, 40, 9, 0));

			m_Camera = scene->GetCamera();

			float hzOffsetX = 15;
			float hzOffsetY = -10;
			float squareOffsetX = 5;
			float squareOffsetY = 15;
			float vtOffsetX = -8;
			float vtOffsetY = 12;
			Gizmos[0] = Gizmo::CreateGizmo(m_GizmoSpritesheet->GetSprite(1), { hzOffsetX, hzOffsetY, 0.0f }, -90.0f, GizmoType::Horizontal);
			Gizmos[1] = Gizmo::CreateGizmo(m_GizmoSpritesheet->GetSprite(4), { vtOffsetX, vtOffsetY, 0.0f }, 0.0f, GizmoType::Vertical);
			Gizmos[2] = Gizmo::CreateGizmo(m_GizmoSpritesheet->GetSprite(0), { squareOffsetX, squareOffsetY, 0.0f }, 0.0f, GizmoType::Free);
			Gizmos[2].Box2D = { glm::vec2(16, 16), glm::vec2(8, 8), glm::vec2(0, -12) };

			Gizmos[3] = Gizmo::CreateGizmo(m_GizmoSpritesheet->GetSprite(2), { hzOffsetX, hzOffsetY, 0.0f }, -90.0f, GizmoType::Horizontal);
			Gizmos[4] = Gizmo::CreateGizmo(m_GizmoSpritesheet->GetSprite(5), { vtOffsetX, vtOffsetY, 0.0f }, 0.0f, GizmoType::Vertical);
			Gizmos[5] = Gizmo::CreateGizmo(m_GizmoSpritesheet->GetSprite(0), { squareOffsetX, squareOffsetY, 0.0f }, 0.0f, GizmoType::Free);
		}

		void GizmoSystem::ImGui()
		{
			//ImGui::Begin("Gizmo debug");
			//ImGui::DragFloat2("Free Move Offset:", &m_FreeMove.m_Offset[0]);
			//ImGui::DragFloat2("Free Move HalfSize: ", &m_FreeMove.m_HalfSize[0]);
			//ImGui::DragFloat2("Free Move BBHalfSize: ", &m_FreeMove.m_Box2D.m_HalfSize[0]);

			//ImGui::DragFloat2("HzMove Offset:", &m_HzMove.m_Offset[0]);
			//ImGui::DragFloat2("HzMove HalfSize: ", &m_HzMove.m_HalfSize[0]);
			//ImGui::DragFloat2("HzMove BBHalfSize: ", &m_HzMove.m_Box2D.m_HalfSize[0]);

			//ImGui::DragFloat2("VtMove Offset:", &m_VtMove.m_Offset[0]);
			//ImGui::DragFloat2("VtMove HalfSize: ", &m_VtMove.m_HalfSize[0]);
			//ImGui::DragFloat2("VtMove BBHalfSize: ", &m_VtMove.m_Box2D.m_HalfSize[0]);
			//ImGui::End();
		}

		void EditorUpdate(Scene* scene, float dt)
		{
			Entity activeEntity = InspectorWindow::GetActiveEntity();
			if (!activeEntity.IsNull())
			{
				ImGui();
				Transform& entityTransform = activeEntity.GetComponent<Transform>();

				if (m_MouseDragging && m_ActiveGizmo >= 0)
				{
					Log::Assert((m_ActiveGizmo >= 0 && m_ActiveGizmo < 6), "Active gizmo out of array bounds.");
					switch (m_Mode)
					{
					case GizmoMode::Translate:
						Gizmo::GizmoManipulateTranslate(Gizmos[m_ActiveGizmo], entityTransform, m_OriginalDragClickPos, m_MouseOffset, m_Camera);
						break;
					case GizmoMode::Rotate:
						Gizmo::GizmoManipulateRotate(Gizmos[m_ActiveGizmo], entityTransform, m_OriginalDragClickPos, m_MouseOffset, m_Camera);
						break;
					case GizmoMode::Scale:
						Gizmo::GizmoManipulateScale(Gizmos[m_ActiveGizmo], entityTransform, m_OriginalDragClickPos, m_OriginalScale, m_Camera);
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
					GizmoData& gizmo = Gizmos[i];
					float cameraZoom = m_Camera->GetZoom() * 2;
					gizmo.Position = entityTransform.m_Position + gizmo.Offset * cameraZoom;
					glm::vec3 boxPos = gizmo.Position + CMath::Vector3From2(gizmo.Box2D.m_Offset) * cameraZoom;
					if (!m_MouseDragging && Physics2D::PointInBox(mousePosWorld, gizmo.Box2D.m_HalfSize * cameraZoom, boxPos, gizmo.SpriteRotation))
					{
						gizmo.Active = true;
						m_HotGizmo = i;
						anyHot = true;
					}
					else if (!m_MouseDragging && m_HotGizmo != i)
					{
						gizmo.Active = false;
					}
					Gizmo::Render(gizmo, m_Camera);
				}
				if (!anyHot)
				{
					m_HotGizmo = -1;
				}
			}
		}

		void OnEvent(Scene* scene, Event& e)
		{
			switch (e.GetType())
			{
			case EventType::KeyPressed:
				e.m_Handled = HandleKeyPress((KeyPressedEvent&)e, scene);
				return;
			case EventType::KeyReleased:
				e.m_Handled = HandleKeyRelease((KeyReleasedEvent&)e, scene);
				return;
			case EventType::MouseButtonPressed:
				e.m_Handled = HandleMouseButtonPressed((MouseButtonPressedEvent&)e, scene);
				return;
			case EventType::MouseButtonReleased:
				e.m_Handled = HandleMouseButtonReleased((MouseButtonReleasedEvent&)e, scene);
				return;
			case EventType::MouseScrolled:
				e.m_Handled = HandleMouseScroll((MouseScrolledEvent&)e, scene);
				return;
			}
		}

		bool GizmoSystem::HandleKeyPress(KeyPressedEvent& e, Scene* scene)
		{
			if (e.GetKeyCode() == COCOA_KEY_S)
			{
				m_Mode = GizmoMode::Scale;
			}
			else if (e.GetKeyCode() == COCOA_KEY_G)
			{
				m_Mode = GizmoMode::Translate;
			}
			return false;
		}

		bool GizmoSystem::HandleKeyRelease(KeyReleasedEvent& e, Scene* scene)
		{

			return false;
		}

		bool GizmoSystem::HandleMouseScroll(MouseScrolledEvent& e, Scene* scene)
		{

			return false;
		}

		bool GizmoSystem::HandleMouseButtonPressed(MouseButtonPressedEvent& e, Scene* scene)
		{
			if (!m_MouseDragging && e.GetMouseButton() == COCOA_MOUSE_BUTTON_LEFT)
			{
				Camera* camera = scene->GetCamera();
				glm::vec2 mousePosWorld = camera->ScreenToOrtho();

				glm::vec2 normalizedMousePos = Input::NormalizedMousePos();
				const PickingTexture& pickingTexture = scene->m_PickingTexture;
				PickingTexture::PixelInfo info = pickingTexture.ReadPixel((uint32)(normalizedMousePos.x * 3840), (uint32)(normalizedMousePos.y * 2160));

				Entity entity = scene->GetEntity(info.m_EntityID);

				Entity selectedEntity = m_HotGizmo == -1 ? entity : InspectorWindow::GetActiveEntity();

				m_OriginalDragClickPos = CMath::Vector3From2(mousePosWorld);
				m_ActiveGizmo = -1;

				if (!selectedEntity.IsNull())
				{
					InspectorWindow::ClearAllEntities();
					InspectorWindow::AddEntity(selectedEntity);
					const Transform& transform = selectedEntity.GetComponent<Transform>();
					m_ActiveGizmo = m_HotGizmo;
					m_MouseDragging = true;
					m_MouseOffset = CMath::Vector3From2(mousePosWorld) - transform.m_Position;
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

		bool GizmoSystem::HandleMouseButtonReleased(MouseButtonReleasedEvent& e, Scene* scene)
		{
			if (m_MouseDragging && e.GetMouseButton() == COCOA_MOUSE_BUTTON_LEFT)
			{
				m_MouseDragging = false;
				CommandHistory::SetNoMergeMostRecent();
			}
			return false;
		}
	}
}