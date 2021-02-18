#include "renderer/Gizmos.h"
#include "core/CocoaEditorApplication.h"
#include "editorWindows/InspectorWindow.h"

#include "cocoa/physics2d/Physics2D.h"
#include "cocoa/renderer/DebugDraw.h"
#include "cocoa/renderer/Camera.h"
#include "cocoa/core/Application.h"
#include "cocoa/core/AssetManager.h"
#include "cocoa/core/Core.h"
#include "cocoa/core/Entity.h"
#include "cocoa/util/CMath.h"
#include "cocoa/util/Settings.h"
#include "cocoa/commands/ICommand.h"
#include "cocoa/systems/RenderSystem.h"
#include "cocoa/components/Spritesheet.h"

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
			data.TextureAssetId = sprite.m_Texture;
			data.Tint = darkTint;
			data.Type = type;
			return data;
		}

		void Render(const GizmoData& data, const Camera& camera)
		{
			float cameraZoom = camera.Zoom * 2;
			if (data.Active)
			{
				DebugDraw::AddSprite(data.TextureAssetId, data.HalfSize * 2.0f * cameraZoom, CMath::Vector2From3(data.Position), data.Tint, data.TexCoordMin, data.TexCoordMax, data.SpriteRotation);
			}
			else
			{
				DebugDraw::AddSprite(data.TextureAssetId, data.HalfSize * 2.0f * cameraZoom, CMath::Vector2From3(data.Position), { 1.0f, 1.0f, 1.0f }, data.TexCoordMin, data.TexCoordMax, data.SpriteRotation);
			}
		}

		void GizmoManipulateTranslate(const GizmoData& data, TransformData& transform, const glm::vec3& originalDragClickPos, const glm::vec3& mouseOffset, const Camera& camera)
		{
			glm::vec3 mousePosWorld = CMath::Vector3From2(NCamera::ScreenToOrtho(camera));
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

			CommandHistory::AddCommand(new ChangeVec3Command(transform.Position, newPos));
		}

		void GizmoManipulateRotate(const GizmoData& data, TransformData& transform, const glm::vec3& startPos, const glm::vec3& mouseOffset, const Camera& camera)
		{

		}

		void GizmoManipulateScale(const GizmoData& data, TransformData& transform, const glm::vec3& originalDragClickPos, const glm::vec3& originalScale, const Camera& camera)
		{
			glm::vec3 mousePosWorld = CMath::Vector3From2(NCamera::ScreenToOrtho(camera));
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

			CommandHistory::AddCommand(new ChangeVec3Command(transform.Scale, originalScale + delta));
		}
	}

	// =================================================================================================
	// Gizmo System
	// =================================================================================================
	namespace GizmoSystem
	{
		// Internal Variables
		static Handle<Texture> m_GizmoTexture = {};
		static Spritesheet m_GizmoSpritesheet;

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
		static bool HandleKeyPress(KeyPressedEvent& e, SceneData& scene);
		static bool HandleKeyRelease(KeyReleasedEvent& e, SceneData& scene);
		static bool HandleMouseButtonPressed(MouseButtonPressedEvent& e, SceneData& scene);
		static bool HandleMouseButtonReleased(MouseButtonReleasedEvent& e, SceneData& scene);
		static bool HandleMouseScroll(MouseScrolledEvent& e, SceneData& scene);

		void Init(SceneData& scene)
		{
			CPath gizmoTexPath = Settings::General::s_EngineAssetsPath;
			NCPath::Join(gizmoTexPath, NCPath::CreatePath("images/gizmos.png"));
			m_GizmoTexture = AssetManager::GetTexture(gizmoTexPath);
			m_GizmoSpritesheet = NSpritesheet::CreateSpritesheet(m_GizmoTexture, 16, 40, 9, 0);

			m_Camera = &scene.SceneCamera;

			float hzOffsetX = 15;
			float hzOffsetY = -10;
			float squareOffsetX = 5;
			float squareOffsetY = 15;
			float vtOffsetX = -8;
			float vtOffsetY = 12;
			Gizmos[0] = Gizmo::CreateGizmo(NSpritesheet::GetSprite(m_GizmoSpritesheet, 1), { hzOffsetX, hzOffsetY, 0.0f }, -90.0f, GizmoType::Horizontal);
			Gizmos[1] = Gizmo::CreateGizmo(NSpritesheet::GetSprite(m_GizmoSpritesheet, 4), { vtOffsetX, vtOffsetY, 0.0f }, 0.0f, GizmoType::Vertical);
			Gizmos[2] = Gizmo::CreateGizmo(NSpritesheet::GetSprite(m_GizmoSpritesheet, 0), { squareOffsetX, squareOffsetY, 0.0f }, 0.0f, GizmoType::Free);
			Gizmos[2].Box2D = { glm::vec2(16, 16), glm::vec2(8, 8), glm::vec2(0, -12) };

			Gizmos[3] = Gizmo::CreateGizmo(NSpritesheet::GetSprite(m_GizmoSpritesheet, 2), { hzOffsetX, hzOffsetY, 0.0f }, -90.0f, GizmoType::Horizontal);
			Gizmos[4] = Gizmo::CreateGizmo(NSpritesheet::GetSprite(m_GizmoSpritesheet, 5), { vtOffsetX, vtOffsetY, 0.0f }, 0.0f, GizmoType::Vertical);
			Gizmos[5] = Gizmo::CreateGizmo(NSpritesheet::GetSprite(m_GizmoSpritesheet, 0), { squareOffsetX, squareOffsetY, 0.0f }, 0.0f, GizmoType::Free);
		}

		void Destroy(SceneData& data)
		{

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

		void EditorUpdate(SceneData& scene, float dt)
		{
			Entity activeEntity = InspectorWindow::GetActiveEntity();
			if (!NEntity::IsNull(activeEntity))
			{
				ImGui();
				TransformData& entityTransform = NEntity::GetComponent<TransformData>(activeEntity);

				if (m_MouseDragging && m_ActiveGizmo >= 0)
				{
					Log::Assert((m_ActiveGizmo >= 0 && m_ActiveGizmo < 6), "Active gizmo out of array bounds.");
					switch (m_Mode)
					{
					case GizmoMode::Translate:
						Gizmo::GizmoManipulateTranslate(Gizmos[m_ActiveGizmo], entityTransform, m_OriginalDragClickPos, m_MouseOffset, *m_Camera);
						break;
					case GizmoMode::Rotate:
						Gizmo::GizmoManipulateRotate(Gizmos[m_ActiveGizmo], entityTransform, m_OriginalDragClickPos, m_MouseOffset, *m_Camera);
						break;
					case GizmoMode::Scale:
						Gizmo::GizmoManipulateScale(Gizmos[m_ActiveGizmo], entityTransform, m_OriginalDragClickPos, m_OriginalScale, *m_Camera);
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

				glm::vec2 mousePosWorld = NCamera::ScreenToOrtho(*m_Camera);
				bool anyHot = false;
				for (int i = start; i < end; i++)
				{
					GizmoData& gizmo = Gizmos[i];
					float cameraZoom = m_Camera->Zoom * 2;
					gizmo.Position = entityTransform.Position + gizmo.Offset * cameraZoom;
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
					Gizmo::Render(gizmo, *m_Camera);
				}
				if (!anyHot)
				{
					m_HotGizmo = -1;
				}
			}
		}

		void OnEvent(SceneData& scene, Event& e)
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

		bool GizmoSystem::HandleKeyPress(KeyPressedEvent& e, SceneData& scene)
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

		bool GizmoSystem::HandleKeyRelease(KeyReleasedEvent& e, SceneData& scene)
		{

			return false;
		}

		bool GizmoSystem::HandleMouseScroll(MouseScrolledEvent& e, SceneData& scene)
		{

			return false;
		}

		bool GizmoSystem::HandleMouseButtonPressed(MouseButtonPressedEvent& e, SceneData& scene)
		{
			if (!m_MouseDragging && e.GetMouseButton() == COCOA_MOUSE_BUTTON_LEFT)
			{
				const Camera& camera = scene.SceneCamera;
				glm::vec2 mousePosWorld = NCamera::ScreenToOrtho(camera);

				glm::vec2 normalizedMousePos = Input::NormalizedMousePos();
				const Framebuffer& mainFramebuffer = RenderSystem::GetMainFramebuffer();
				uint32 pixel = NFramebuffer::ReadPixelUint32(mainFramebuffer, 1, (uint32)(normalizedMousePos.x * 3840), (uint32)(normalizedMousePos.y * 2160));

				Entity entity = Scene::GetEntity(scene, pixel);
				Entity selectedEntity = m_HotGizmo == -1 ? entity : InspectorWindow::GetActiveEntity();

				m_OriginalDragClickPos = CMath::Vector3From2(mousePosWorld);
				m_ActiveGizmo = -1;

				if (!NEntity::IsNull(selectedEntity))
				{
					InspectorWindow::ClearAllEntities();
					InspectorWindow::AddEntity(selectedEntity);
					const TransformData& transform = NEntity::GetComponent<TransformData>(selectedEntity);
					m_ActiveGizmo = m_HotGizmo;
					m_MouseDragging = true;
					m_MouseOffset = CMath::Vector3From2(mousePosWorld) - transform.Position;
					m_OriginalScale = transform.Scale;
				}
				else
				{
					InspectorWindow::ClearAllEntities();
					m_ActiveGizmo = -1;
				}
			}

			return false;
		}

		bool GizmoSystem::HandleMouseButtonReleased(MouseButtonReleasedEvent& e, SceneData& scene)
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