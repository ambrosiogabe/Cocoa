#include "core/LevelEditorSystem.h"
#include "core/CocoaEditorApplication.h"
#include "gui/FontAwesome.h"
#include "gui/ImGuiExtended.h"
#include "editorWindows/InspectorWindow.h"
#include "editorWindows/SceneHierarchyWindow.h"
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
		void init(SceneData& scene)
		{
			InitComponentIds(scene);
			tmpScriptDll = Settings::General::engineExeDirectory;
			tmpScriptDll.join(CPath::create("ScriptModuleTmp.dll"));
			scriptDll = Settings::General::engineExeDirectory;
			scriptDll.join(CPath::create("ScriptModule.dll"));
			initImGui = false;

			Logger::Assert(!Scene::isValid(scene, m_CameraEntity), "Tried to initialize level editor system twice.");
			// Create editor camera's framebuffer
			Framebuffer editorCameraFramebuffer;
			editorCameraFramebuffer.width = 3840;
			editorCameraFramebuffer.height = 2160;
			editorCameraFramebuffer.includeDepthStencil = false;
			Texture color0;
			color0.internalFormat = ByteFormat::RGB;
			color0.externalFormat = ByteFormat::RGB;
			color0.magFilter = FilterMode::Linear;
			color0.minFilter = FilterMode::Linear;
			color0.wrapS = WrapMode::Repeat;
			color0.wrapT = WrapMode::Repeat;
			NFramebuffer::addColorAttachment(editorCameraFramebuffer, color0);
			Texture color1;
			color1.internalFormat = ByteFormat::R32UI;
			color1.externalFormat = ByteFormat::RED_INTEGER;
			NFramebuffer::addColorAttachment(editorCameraFramebuffer, color1);
			NFramebuffer::generate(editorCameraFramebuffer);

			Camera editorCamera = NCamera::createCamera(editorCameraFramebuffer);

			m_CameraEntity = Scene::createEntity(scene);
			NEntity::addComponent<Camera>(m_CameraEntity, editorCamera);
			NEntity::addComponent<NoSerialize>(m_CameraEntity);
		}

		void destroy(SceneData& scene)
		{

		}

		static void InitComponentIds(SceneData& scene)
		{
			NEntity::registerComponentType<TransformData>();
			NEntity::registerComponentType<Tag>();
			NEntity::registerComponentType<SpriteRenderer>();
			NEntity::registerComponentType<FontRenderer>();
			NEntity::registerComponentType<Rigidbody2D>();
			NEntity::registerComponentType<Box2D>();
			NEntity::registerComponentType<Circle>();
			NEntity::registerComponentType<AABB>();
			NEntity::registerComponentType<Camera>();
			NEntity::registerComponentType<NoSerialize>();
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

		void editorUpdate(SceneData& scene, float dt)
		{
			if (!initImGui)
			{
				ScriptSystem::initImGui(ImGui::GetCurrentContext());
				initImGui = true;
			}

			if (File::isFile(tmpScriptDll))
			{
				Scene::save(scene, Settings::General::currentScene);
				EditorLayer::saveProject();

				// This should free the scripts too so we can delete the old dll
				Scene::freeResources(scene);

				// Now copy new dll and reload the scene
				File::deleteFile(scriptDll);
				File::copyFile(tmpScriptDll, CPath::create(scriptDll.getDirectory(-1)), "ScriptModule");
				Scene::load(scene, Settings::General::currentScene);

				// Then delete temporary file of new dll
				File::deleteFile(tmpScriptDll);
			}

			Camera& camera = NEntity::getComponent<Camera>(m_CameraEntity);
			// TODO: Make an OnRender function and call this in there
			NCamera::clearColorUint32(camera, 1, (uint32)-1);
			TransformData& cameraTransform = NEntity::getComponent<TransformData>(m_CameraEntity);
			if (m_IsDragging)
			{
				glm::vec3 mousePosWorld = CMath::vector3From2(NCamera::screenToOrtho(camera));
				glm::vec3 delta = m_OriginalDragClickPos - mousePosWorld;
				// TODO: Make this an editor setting
				static const float sharpness = 15.0f;
				cameraTransform.position = lerp(cameraTransform.position, cameraTransform.position + delta, dt * sharpness);
			}

			// Draw grid lines
			if (Settings::Editor::drawGrid)
			{
				float cameraZoom = camera.zoom;
				float gridWidth = Settings::Editor::gridSize.x;
				float gridHeight = Settings::Editor::gridSize.y;
				float projectionWidth = camera.projectionSize.x;
				float projectionHeight = camera.projectionSize.y;

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
					DebugDraw::addLine2D(from, to, Settings::Editor::gridStrokeWidth, Settings::Editor::gridColor, 1, false);

					if (i <= xLinesNeeded)
					{
						float x0 = firstX - gridWidth;
						float x1 = firstX + projectionWidth + gridWidth;
						glm::vec2 from2(x0, y);
						glm::vec2 to2(x1, y);
						DebugDraw::addLine2D(from2, to2, Settings::Editor::gridStrokeWidth, Settings::Editor::gridColor, 1, false);
					}
				}
			}

		}

		Camera& getCamera()
		{
			return NEntity::getComponent<Camera>(m_CameraEntity);;
		}

		// TODO: Move this into CImGui?
		static ImVec4 From(const glm::vec4& vec4)
		{
			return ImVec4(vec4.x, vec4.y, vec4.z, vec4.w);
		}

		void LevelEditorSystem::onEvent(SceneData& scene, Event& e)
		{
			switch (e.getType())
			{
			case EventType::KeyPressed:
				e.handled = HandleKeyPress((KeyPressedEvent&)e, scene);
				return;
			case EventType::KeyReleased:
				e.handled = HandleKeyRelease((KeyReleasedEvent&)e, scene);
				return;
			case EventType::MouseButtonPressed:
				e.handled = HandleMouseButtonPressed((MouseButtonPressedEvent&)e, scene);
				return;
			case EventType::MouseButtonReleased:
				e.handled = HandleMouseButtonReleased((MouseButtonReleasedEvent&)e, scene);
				return;
			case EventType::MouseScrolled:
				e.handled = HandleMouseScroll((MouseScrolledEvent&)e, scene);
				return;
			}
		}

		bool HandleKeyPress(KeyPressedEvent& e, SceneData& scene)
		{
			if (e.getKeyCode() == COCOA_KEY_LEFT_CONTROL)
			{
				m_ControlModifierPressed = true;
			}

			if (m_ControlModifierPressed)
			{
				if (e.getKeyCode() == COCOA_KEY_Z)
				{
					CommandHistory::undo();
				}

				if (e.getKeyCode() == COCOA_KEY_R)
				{
					CommandHistory::redo();
				}

				if (e.getKeyCode() == COCOA_KEY_S)
				{
					Scene::save(scene, Settings::General::currentScene);
					EditorLayer::saveProject();
				}

				if (e.getKeyCode() == COCOA_KEY_D)
				{
					Entity activeEntity = InspectorWindow::getActiveEntity();
					if (!NEntity::isNull(activeEntity))
					{
						Entity duplicated = Scene::duplicateEntity(scene, activeEntity);
						InspectorWindow::clearAllEntities();
						InspectorWindow::addEntity(duplicated);
						SceneHierarchyWindow::addNewEntity(duplicated);
					}
				}
			}

			if (e.getKeyCode() == COCOA_KEY_DELETE)
			{
				Entity activeEntity = InspectorWindow::getActiveEntity();
				if (!NEntity::isNull(activeEntity))
				{
					SceneHierarchyWindow::deleteEntity(activeEntity);
					Scene::deleteEntity(scene, activeEntity);
					InspectorWindow::clearAllEntities();
				}
			}

			return false;
		}

		bool HandleKeyRelease(KeyReleasedEvent& e, SceneData& scene)
		{
			if (e.getKeyCode() == COCOA_KEY_LEFT_CONTROL)
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

			float yOffset = -e.getYOffset();
			if (yOffset != 0)
			{
				Camera& camera = NEntity::getComponent<Camera>(m_CameraEntity);
				if (step < 0)
				{
					step = (glm::log(camera.zoom) - logMinZoom) * ((maxSteps - 1) / (logMaxZoom - logMinZoom));
				}

				step = glm::clamp(yOffset < 0 ? step - 1.0f : step + 1.0f, 0.0f, maxSteps);
				float logZoom = logMinZoom + (logMaxZoom - logMinZoom) * step / (maxSteps - 1);
				camera.zoom = glm::exp(logZoom);
				NCamera::adjustPerspective(camera);
			}

			return false;
		}

		bool HandleMouseButtonPressed(MouseButtonPressedEvent& e, SceneData& scene)
		{
			static float speed = 500.0f;
			if (!m_IsDragging && e.getMouseButton() == COCOA_MOUSE_BUTTON_MIDDLE)
			{
				m_IsDragging = true;
				const Camera& camera = NEntity::getComponent<Camera>(m_CameraEntity);
				const TransformData& transform = NEntity::getComponent<TransformData>(m_CameraEntity);
				m_OriginalCameraPos = transform.position;
				m_OriginalDragClickPos = CMath::vector3From2(NCamera::screenToOrtho(camera));
			}

			return false;
		}

		bool HandleMouseButtonReleased(MouseButtonReleasedEvent& e, SceneData& scene)
		{
			if (m_IsDragging && e.getMouseButton() == COCOA_MOUSE_BUTTON_MIDDLE)
			{
				m_IsDragging = false;
			}
			return false;
		}

		void serialize(json& j)
		{
			Camera& editorCamera = NEntity::getComponent<Camera>(m_CameraEntity);
			j["EditorCamera"] = { 
				{"Components", {}} 
			};
			Scene::serializeEntity(&j["EditorCamera"], m_CameraEntity);
		}

		void deserialize(const json& j, SceneData& scene)
		{
			Logger::Assert(Scene::isValid(scene, m_CameraEntity), "Invalid level editor system. It does not have a camera initialized.");
			if (j.contains("EditorCamera") && Scene::isValid(scene, m_CameraEntity))
			{
				Scene::deleteEntity(scene, m_CameraEntity);
				// TODO: This is kind of hacky, there's probably a better way to do it
				Scene::deserializeEntities(j["EditorCamera"], scene);
				NEntity::addComponent<NoSerialize>(m_CameraEntity);
			}
		}
	}
}