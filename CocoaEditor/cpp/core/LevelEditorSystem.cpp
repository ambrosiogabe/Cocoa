#include "core/LevelEditorSystem.h"
#include "core/CocoaEditorApplication.h"
#include "gui/FontAwesome.h"
#include "gui/ImGuiExtended.h"
#include "editorWindows/InspectorWindow.h"

#include "cocoa/systems/ScriptSystem.h"
#include "cocoa/core/Application.h"
#include "cocoa/events/Input.h"
#include "cocoa/commands/ICommand.h"
#include "cocoa/renderer/DebugDraw.h"
#include "cocoa/util/CMath.h"
#include "cocoa/util/Settings.h"
#include "cocoa/file/IFile.h"

#include <imgui.h>

#undef CopyFile;
#undef DeleteFile;

namespace Cocoa
{
	static CPath tmpScriptDll;
	static CPath scriptDll;

	static bool initImGui = false;

	void LevelEditorSystem::Start()
	{
		tmpScriptDll = Settings::General::s_EngineExeDirectory + CPath("ScriptModuleTmp.dll");
		scriptDll = Settings::General::s_EngineExeDirectory + CPath("ScriptModule.dll");
		auto view = m_Scene->GetRegistry().view<Transform>();
		initImGui = false;
		for (Entity entity : view)
		{
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
		}
	}

	void LevelEditorSystem::EditorUpdate(float dt)
	{
		if (!initImGui)
		{
			if (m_ScriptSystem->m_InitImGui)
			{
				m_ScriptSystem->m_InitImGui(ImGui::GetCurrentContext());
			}
			initImGui = true;
		}

		if (IFile::IsFile(tmpScriptDll))
		{
			m_Scene->Save(Settings::General::s_CurrentScene);
			EditorLayer::SaveProject();
			m_ScriptSystem->FreeScriptLibrary();

			IFile::DeleteFile(scriptDll);
			IFile::CopyFile(tmpScriptDll, scriptDll.GetDirectory(-1), "ScriptModule");
			m_ScriptSystem->Reload();
			if (m_ScriptSystem->m_InitImGui)
			{
				m_ScriptSystem->m_InitImGui(ImGui::GetCurrentContext());
			}
			m_Scene->LoadScriptsOnly(Settings::General::s_CurrentScene);

			IFile::DeleteFile(tmpScriptDll);
		}

		if (m_IsDragging)
		{
			Camera* camera = m_Scene->GetCamera();
			glm::vec3 mousePosWorld = CMath::Vector3From2(camera->ScreenToOrtho());
			glm::vec3 delta = m_OriginalDragClickPos - mousePosWorld;
			delta *= 0.8f;
			camera->GetTransform().m_Position = m_OriginalCameraPos + delta;
		}

		// Draw grid lines
		if (Settings::General::s_DrawGrid)
		{
			Transform& cameraTransform = m_Scene->GetCamera()->GetTransform();
			float cameraZoom = m_Scene->GetCamera()->GetZoom();
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

	static ImVec4 From(const glm::vec4& vec4)
	{
		return ImVec4(vec4.x, vec4.y, vec4.z, vec4.w);
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
				m_Scene->Save(Settings::General::s_CurrentScene);
				EditorLayer::SaveProject();
			}

			if (e.GetKeyCode() == COCOA_KEY_D)
			{
				Entity activeEntity = InspectorWindow::GetActiveEntity();
				if (!activeEntity.IsNull())
				{
					Entity duplicated = m_Scene->DuplicateEntity(activeEntity);
					InspectorWindow::ClearAllEntities();
					InspectorWindow::AddEntity(duplicated);
				}
			}
		}

		return false;
	}

	bool LevelEditorSystem::HandleKeyRelease(KeyReleasedEvent& e)
	{
		if (e.GetKeyCode() == COCOA_KEY_LEFT_CONTROL)
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
			Camera* camera = m_Scene->GetCamera();
			//float speed = 500.0f * camera->GetZoom();
			camera->SetZoom(camera->GetZoom() + (yOffset * 0.05f));
		}

		return false;
	}

	bool LevelEditorSystem::HandleMouseButtonPressed(MouseButtonPressedEvent& e)
	{
		static float speed = 500.0f;
		if (!m_IsDragging && e.GetMouseButton() == COCOA_MOUSE_BUTTON_MIDDLE)
		{
			m_IsDragging = true;
			Camera* camera = m_Scene->GetCamera();
			m_OriginalCameraPos = camera->GetTransform().m_Position;
			m_OriginalDragClickPos = CMath::Vector3From2(camera->ScreenToOrtho());
			//m_DragClickOffset = CMath::Vector3From2(camera->ScreenToOrtho()) - camera->GetTransform().m_Position;
		}

		return false;
	}

	bool LevelEditorSystem::HandleMouseButtonReleased(MouseButtonReleasedEvent& e)
	{
		if (m_IsDragging && e.GetMouseButton() == COCOA_MOUSE_BUTTON_MIDDLE)
		{
			m_IsDragging = false;
		}
		return false;
	}
}