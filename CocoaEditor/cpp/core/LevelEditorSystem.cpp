#include "core/LevelEditorSystem.h"
#include "core/CocoaEditorApplication.h"
#include "gui/FontAwesome.h"
#include "gui/ImGuiExtended.h"
#include "editorWindows/InspectorWindow.h"
#include "editorWindows/SceneHeirarchyWindow.h"
#include "util/Settings.h"

#include "cocoa/systems/ScriptSystem.h"
#include "cocoa/core/Application.h"
#include "cocoa/events/Input.h"
#include "cocoa/commands/ICommand.h"
#include "cocoa/renderer/DebugDraw.h"
#include "cocoa/renderer/Camera.h"
#include "cocoa/util/CMath.h"
#include "cocoa/util/Settings.h"
#include "cocoa/file/File.h"
#include "cocoa/components/Tag.h"
#include "cocoa/components/SpriteRenderer.h"
#include "cocoa/components/FontRenderer.h"
#include "cocoa/components/NoSerialize.h"
#include "cocoa/physics2d/PhysicsComponents.h"

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

		static Entity m_CameraEntity;

		// Forward Declarations
		static bool HandleKeyPress(KeyPressedEvent& e, SceneData& scene);
		static bool HandleKeyRelease(KeyReleasedEvent& e, SceneData& scene);
		static bool HandleMouseButtonPressed(MouseButtonPressedEvent& e, SceneData& scene);
		static bool HandleMouseButtonReleased(MouseButtonReleasedEvent& e, SceneData& scene);
		static bool HandleMouseScroll(MouseScrolledEvent& e, SceneData& scene);

		static void InitComponentIds(SceneData& scene);
		void Init(SceneData& scene)
		{
			InitComponentIds(scene);
			tmpScriptDll = Settings::General::s_EngineExeDirectory;
			tmpScriptDll.Join(CPath::Create("ScriptModuleTmp.dll"));
			scriptDll = Settings::General::s_EngineExeDirectory;
			scriptDll.Join(CPath::Create("ScriptModule.dll"));
			initImGui = false;

			Logger::Assert(!Scene::IsValid(scene, m_CameraEntity), "Tried to initialize level editor system twice.");
			// Create editor camera's framebuffer
			Framebuffer editorCameraFramebuffer;
			editorCameraFramebuffer.Width = 3840;
			editorCameraFramebuffer.Height = 2160;
			editorCameraFramebuffer.IncludeDepthStencil = false;
			Texture color0;
			color0.InternalFormat = ByteFormat::RGB;
			color0.ExternalFormat = ByteFormat::RGB;
			color0.MagFilter = FilterMode::Linear;
			color0.MinFilter = FilterMode::Linear;
			color0.WrapS = WrapMode::Repeat;
			color0.WrapT = WrapMode::Repeat;
			NFramebuffer::AddColorAttachment(editorCameraFramebuffer, color0);
			Texture color1;
			color1.InternalFormat = ByteFormat::R32UI;
			color1.ExternalFormat = ByteFormat::RED_INTEGER;
			NFramebuffer::AddColorAttachment(editorCameraFramebuffer, color1);
			NFramebuffer::Generate(editorCameraFramebuffer);

			Camera editorCamera = NCamera::CreateCamera(editorCameraFramebuffer);

			m_CameraEntity = Scene::CreateEntity(scene);
			NEntity::AddComponent<Camera>(m_CameraEntity, editorCamera);
			NEntity::AddComponent<NoSerialize>(m_CameraEntity);
		}

		void Destroy(SceneData& scene)
		{

		}

		static void InitComponentIds(SceneData& scene)
		{
			NEntity::RegisterComponentType<TransformData>();
			NEntity::RegisterComponentType<Tag>();
			NEntity::RegisterComponentType<SpriteRenderer>();
			NEntity::RegisterComponentType<FontRenderer>();
			NEntity::RegisterComponentType<Rigidbody2D>();
			NEntity::RegisterComponentType<Box2D>();
			NEntity::RegisterComponentType<Circle>();
			NEntity::RegisterComponentType<AABB>();
			NEntity::RegisterComponentType<Camera>();
			NEntity::RegisterComponentType<NoSerialize>();
		}

		static float lerp(float a, float b, float t)
		{
			return a + t * (b - a);
		}

		static glm::vec3 lerp(glm::vec3& a, glm::vec3& b, float t)
		{
			return {
				lerp(a.x, b.x, t),
				lerp(a.y, b.y, t),
				lerp(a.z, b.z, t)
			};
		}

		void EditorUpdate(SceneData& scene, float dt)
		{
			if (!initImGui)
			{
				ScriptSystem::InitImGui(ImGui::GetCurrentContext());
				initImGui = true;
			}

			if (File::IsFile(tmpScriptDll))
			{
				Scene::Save(scene, Settings::General::s_CurrentScene);
				EditorLayer::saveProject();

				// This should free the scripts too so we can delete the old dll
				Scene::FreeResources(scene);

				// Now copy new dll and reload the scene
				File::DeleteFile(scriptDll);
				File::CopyFile(tmpScriptDll, CPath::Create(scriptDll.GetDirectory(-1)), "ScriptModule");
				Scene::Load(scene, Settings::General::s_CurrentScene);

				// Then delete temporary file of new dll
				File::DeleteFile(tmpScriptDll);
			}

			Camera& camera = NEntity::GetComponent<Camera>(m_CameraEntity);
			// TODO: Make an OnRender function and call this in there
			NCamera::ClearColorUint32(camera, 1, (uint32)-1);
			TransformData& cameraTransform = NEntity::GetComponent<TransformData>(m_CameraEntity);
			if (m_IsDragging)
			{
				glm::vec3 mousePosWorld = CMath::Vector3From2(NCamera::ScreenToOrtho(camera));
				glm::vec3 delta = m_OriginalDragClickPos - mousePosWorld;
				// TODO: Make this an editor setting
				static const float sharpness = 15.0f;
				cameraTransform.position = lerp(cameraTransform.position, cameraTransform.position + delta, dt * sharpness);
			}

			// Draw grid lines
			if (Settings::Editor::DrawGrid)
			{
				float cameraZoom = camera.Zoom;
				float gridWidth = Settings::Editor::GridSize.x;
				float gridHeight = Settings::Editor::GridSize.y;
				float projectionWidth = camera.ProjectionSize.x;
				float projectionHeight = camera.ProjectionSize.y;

				float firstX = (float)(((int)(cameraTransform.position.x - cameraZoom * projectionWidth / 2.0f) / gridWidth) - 1) * (float)gridWidth;
				float firstY = (float)(((int)(cameraTransform.position.y - cameraZoom * projectionHeight / 2.0f) / gridHeight) - 1) * (float)gridHeight;

				int yLinesNeeded = (int)((cameraZoom * projectionWidth + gridWidth) / gridWidth);
				int xLinesNeeded = (int)((cameraZoom * projectionHeight + gridHeight) / gridHeight);

				for (int i = 0; i < yLinesNeeded; i++)
				{
					float x = (i * gridWidth) + firstX + (gridWidth / 2.0f);
					float y = (i * gridHeight) + firstY + (gridHeight / 2.0f);

					float y0 = firstY - gridHeight;
					float y1 = firstY + projectionHeight + gridHeight;
					glm::vec2 from(x, y0);
					glm::vec2 to(x, y1);
					DebugDraw::AddLine2D(from, to, Settings::Editor::GridStrokeWidth, Settings::Editor::GridColor, 1, false);

					if (i <= xLinesNeeded)
					{
						float x0 = firstX - gridWidth;
						float x1 = firstX + projectionWidth + gridWidth;
						glm::vec2 from2(x0, y);
						glm::vec2 to2(x1, y);
						DebugDraw::AddLine2D(from2, to2, Settings::Editor::GridStrokeWidth, Settings::Editor::GridColor, 1, false);
					}
				}
			}

		}

		Camera& GetCamera()
		{
			return NEntity::GetComponent<Camera>(m_CameraEntity);;
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
					CommandHistory::undo();
				}

				if (e.GetKeyCode() == COCOA_KEY_R)
				{
					CommandHistory::redo();
				}

				if (e.GetKeyCode() == COCOA_KEY_S)
				{
					Scene::Save(scene, Settings::General::s_CurrentScene);
					EditorLayer::saveProject();
				}

				if (e.GetKeyCode() == COCOA_KEY_D)
				{
					Entity activeEntity = InspectorWindow::GetActiveEntity();
					if (!NEntity::IsNull(activeEntity))
					{
						Entity duplicated = Scene::DuplicateEntity(scene, activeEntity);
						InspectorWindow::ClearAllEntities();
						InspectorWindow::AddEntity(duplicated);
						SceneHeirarchyWindow::AddNewEntity(duplicated);
					}
				}
			}

			if (e.GetKeyCode() == COCOA_KEY_DELETE)
			{
				Entity activeEntity = InspectorWindow::GetActiveEntity();
				if (!NEntity::IsNull(activeEntity))
				{
					SceneHeirarchyWindow::DeleteEntity(activeEntity);
					Scene::DeleteEntity(scene, activeEntity);
					InspectorWindow::ClearAllEntities();
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
			static const float minZoom = 0.3f;
			static const float logMinZoom = glm::log(minZoom);
			static const float maxZoom = 100.0f;
			static const float logMaxZoom = glm::log(maxZoom);
			// TODO: Make this an editor setting (maxSteps should be the setting)
			static const float maxSteps = 100;
			static float step = -1;
			static float cameraSensitivity = 0.5f;

			float yOffset = -e.GetYOffset();
			if (yOffset != 0)
			{
				Camera& camera = NEntity::GetComponent<Camera>(m_CameraEntity);
				if (step < 0)
				{
					step = (glm::log(camera.Zoom) - logMinZoom) * ((maxSteps - 1) / (logMaxZoom - logMinZoom));
				}

				step = glm::clamp(yOffset < 0 ? step - 1.0f : step + 1.0f, 0.0f, maxSteps);
				float logZoom = logMinZoom + (logMaxZoom - logMinZoom) * step / (maxSteps - 1);
				camera.Zoom = glm::exp(logZoom);
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
				const Camera& camera = NEntity::GetComponent<Camera>(m_CameraEntity);
				const TransformData& transform = NEntity::GetComponent<TransformData>(m_CameraEntity);
				m_OriginalCameraPos = transform.position;
				m_OriginalDragClickPos = CMath::Vector3From2(NCamera::ScreenToOrtho(camera));
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

		void Serialize(json& j)
		{
			Camera& editorCamera = NEntity::GetComponent<Camera>(m_CameraEntity);
			j["EditorCamera"] = { 
				{"Components", {}} 
			};
			Scene::SerializeEntity(&j["EditorCamera"], m_CameraEntity);
		}

		void Deserialize(const json& j, SceneData& scene)
		{
			Logger::Assert(Scene::IsValid(scene, m_CameraEntity), "Invalid level editor system. It does not have a camera initialized.");
			if (j.contains("EditorCamera") && Scene::IsValid(scene, m_CameraEntity))
			{
				Scene::DeleteEntity(scene, m_CameraEntity);
				// TODO: This is kind of hacky, there's probably a better way to do it
				Scene::DeserializeEntities(j["EditorCamera"], scene);
				NEntity::AddComponent<NoSerialize>(m_CameraEntity);
			}
		}
	}
}