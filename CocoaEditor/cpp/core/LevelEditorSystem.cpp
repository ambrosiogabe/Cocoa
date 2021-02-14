#include "core/LevelEditorSystem.h"
#include "core/CocoaEditorApplication.h"
#include "gui/FontAwesome.h"
#include "gui/ImGuiExtended.h"
#include "editorWindows/InspectorWindow.h"
#include "util/Settings.h"

#include "cocoa/systems/ScriptSystem.h"
#include "cocoa/core/Application.h"
#include "cocoa/events/Input.h"
#include "cocoa/commands/ICommand.h"
#include "cocoa/renderer/DebugDraw.h"
#include "cocoa/renderer/Camera.h"
#include "cocoa/util/CMath.h"
#include "cocoa/util/Settings.h"
#include "cocoa/file/IFile.h"

#include <imgui.h>

#ifdef CopyFile
#undef CopyFile;
#endif 
#ifdef DeleteFile 
#undef DeleteFile;
#endif

namespace Cocoa
{
	namespace LevelEditorSystem
	{
		// Internal Variables
		static float m_KeyDebounceTime = 0.1f;
		static float m_KeyDebounceLeft = 0.0f;

		static bool m_IsDragging = false;
		static bool m_ControlModifierPressed = false;
		static bool initImGui = false;

		static glm::vec3 m_OriginalDragClickPos = glm::vec3();
		static glm::vec3 m_OriginalCameraPos = glm::vec3();

		static CPath tmpScriptDll;
		static CPath scriptDll;

		// Forward Declarations
		static bool HandleKeyPress(KeyPressedEvent& e, SceneData& scene);
		static bool HandleKeyRelease(KeyReleasedEvent& e, SceneData& scene);
		static bool HandleMouseButtonPressed(MouseButtonPressedEvent& e, SceneData& scene);
		static bool HandleMouseButtonReleased(MouseButtonReleasedEvent& e, SceneData& scene);
		static bool HandleMouseScroll(MouseScrolledEvent& e, SceneData& scene);

		void Start(SceneData& scene)
		{
			tmpScriptDll = Settings::General::s_EngineExeDirectory;
			NCPath::Join(tmpScriptDll, NCPath::CreatePath("ScriptModuleTmp.dll"));
			scriptDll = Settings::General::s_EngineExeDirectory;
			NCPath::Join(scriptDll, NCPath::CreatePath("ScriptModule.dll"));
			auto view = scene.Registry.view<TransformData>();
			initImGui = false;
			//for (CEntity entity : view)
			//{
				//Transform& transform = entity.GetComponent<Transform>();
				//if (!transform.m_Previous.IsNull())
				//{
				//	Transform& prevTransform = transform.m_Previous.GetComponent<Transform>();
				//	prevTransform.m_Next = entity;

				//	if (!transform.m_Parent.IsNull())
				//	{
				//		Transform& parentTransform = transform.m_Parent.GetComponent<Transform>();
				//		if (!transform.m_First.IsNull())
				//		{
				//			parentTransform.m_First = entity;
				//		}
				//	}
				//}
			//}
		}

		void EditorUpdate(SceneData& scene, float dt)
		{
			if (!initImGui)
			{
				ScriptSystem::InitImGui(ImGui::GetCurrentContext());
				initImGui = true;
			}

			if (IFile::IsFile(tmpScriptDll))
			{
				Scene::Save(scene, Settings::General::s_CurrentScene);
				EditorLayer::SaveProject();
				ScriptSystem::FreeScriptLibrary();

				IFile::DeleteFile(scriptDll);
				IFile::CopyFile(tmpScriptDll, NCPath::CreatePath(NCPath::GetDirectory(scriptDll, -1)), "ScriptModule");
				ScriptSystem::Reload();
				ScriptSystem::InitImGui(ImGui::GetCurrentContext());
				Scene::LoadScriptsOnly(scene, Settings::General::s_CurrentScene);

				IFile::DeleteFile(tmpScriptDll);
			}

			if (m_IsDragging)
			{
				Camera& camera = scene.SceneCamera;
				glm::vec3 mousePosWorld = CMath::Vector3From2(NCamera::ScreenToOrtho(camera));
				glm::vec3 delta = m_OriginalDragClickPos - mousePosWorld;
				delta *= 0.8f;
				camera.Transform.Position = m_OriginalCameraPos + delta;
			}

			// Draw grid lines
			if (Settings::Editor::DrawGrid)
			{
				TransformData& cameraTransform = scene.SceneCamera.Transform;
				float cameraZoom = scene.SceneCamera.Zoom;
				int gridWidth = Settings::Editor::GridSize.x;// *cameraZoom;
				int gridHeight = Settings::Editor::GridSize.y;// *cameraZoom;

				float firstX = (float)(((int)(cameraTransform.Position.x - cameraZoom * 1920.0f / 2.0f) / gridWidth) - 1) * (float)gridWidth;
				float firstY = (float)(((int)(cameraTransform.Position.y - cameraZoom * 1080.0f / 2.0f) / gridHeight) - 1) * (float)gridHeight;

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

		// TODO: Move this into CImGui?
		static ImVec4 From(const glm::vec4& vec4)
		{
			return ImVec4(vec4.x, vec4.y, vec4.z, vec4.w);
		}

		void LevelEditorSystem::OnEvent(SceneData& scene, Event& e)
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

		bool HandleKeyPress(KeyPressedEvent& e, SceneData& scene)
		{
			if (e.GetKeyCode() == COCOA_KEY_LEFT_CONTROL)
			{
				m_ControlModifierPressed = true;
			}

			if (m_ControlModifierPressed)
			{
				if (e.GetKeyCode() == COCOA_KEY_Z)
				{
					CommandHistory::Undo();
				}

				if (e.GetKeyCode() == COCOA_KEY_R)
				{
					CommandHistory::Redo();
				}

				if (e.GetKeyCode() == COCOA_KEY_S)
				{
					Scene::Save(scene, Settings::General::s_CurrentScene);
					EditorLayer::SaveProject();
				}

				if (e.GetKeyCode() == COCOA_KEY_D)
				{
					Entity activeEntity = InspectorWindow::GetActiveEntity();
					if (!NEntity::IsNull(activeEntity))
					{
						Entity duplicated = Scene::DuplicateEntity(scene, activeEntity);
						InspectorWindow::ClearAllEntities();
						InspectorWindow::AddEntity(duplicated);
					}
				}
			}

			return false;
		}

		bool HandleKeyRelease(KeyReleasedEvent& e, SceneData& scene)
		{
			if (e.GetKeyCode() == COCOA_KEY_LEFT_CONTROL)
			{
				m_ControlModifierPressed = false;
			}

			return false;
		}

		bool HandleMouseScroll(MouseScrolledEvent& e, SceneData& scene)
		{
			float yOffset = -e.GetYOffset();
			if (yOffset != 0)
			{
				Camera& camera = scene.SceneCamera;
				//float speed = 500.0f * camera->GetZoom();
				camera.Zoom = camera.Zoom + (yOffset * 0.05f);
				NCamera::AdjustPerspective(camera);
			}

			return false;
		}

		bool HandleMouseButtonPressed(MouseButtonPressedEvent& e, SceneData& scene)
		{
			static float speed = 500.0f;
			if (!m_IsDragging && e.GetMouseButton() == COCOA_MOUSE_BUTTON_MIDDLE)
			{
				m_IsDragging = true;
				const Camera& camera = scene.SceneCamera;
				m_OriginalCameraPos = camera.Transform.Position;
				m_OriginalDragClickPos = CMath::Vector3From2(NCamera::ScreenToOrtho(camera));
				//m_DragClickOffset = CMath::Vector3From2(camera->ScreenToOrtho()) - camera->GetTransform().m_Position;
			}

			return false;
		}

		bool HandleMouseButtonReleased(MouseButtonReleasedEvent& e, SceneData& scene)
		{
			if (m_IsDragging && e.GetMouseButton() == COCOA_MOUSE_BUTTON_MIDDLE)
			{
				m_IsDragging = false;
			}
			return false;
		}
	}
}