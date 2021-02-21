#include "Cocoa.h"

#include "core/CocoaEditorApplication.h"
#include "core/LevelEditorSceneInitializer.h"
#include "core/ImGuiLayer.h"
#include "gui/ImGuiHeader.h"
#include "editorWindows/InspectorWindow.h"
#include "nativeScripting/SourceFileWatcher.h"
#include "renderer/Gizmos.h"

#include "cocoa/file/File.h"
#include "cocoa/util/Settings.h"
#include "cocoa/systems/RenderSystem.h"
#include "cocoa/core/AssetManager.h"
#include "cocoa/core/Memory.h"

#include <glad/glad.h>
#include <nlohmann/json.hpp>

#ifdef CopyFile
#undef CopyFile
#endif

namespace Cocoa
{
	// ===================================================================================
	// Editor Layer
	// ===================================================================================
	namespace EditorLayer
	{
		// Internal Variables
		static std::shared_ptr<SourceFileWatcher> m_SourceFileWatcher;
		static bool m_ProjectLoaded = false;
		static bool m_EditorUpdate = true;

		// Forward Declarations

		void Init()
		{
			Settings::General::s_EngineExeDirectory = NCPath::CreatePath(NCPath::GetDirectory(File::GetExecutableDirectory(), -1));
			Settings::General::s_EngineSourceDirectory = NCPath::CreatePath(NCPath::GetDirectory(File::GetExecutableDirectory(), -4));
			Log::Info("%s", Settings::General::s_EngineExeDirectory.Path.c_str());

			// Set the assets path as CWD (which should be where the exe is currently located)
			Settings::General::s_EngineAssetsPath = File::GetCwd();
			NCPath::Join(Settings::General::s_EngineAssetsPath, NCPath::CreatePath("assets"));
			Settings::General::s_ImGuiConfigPath = Settings::General::s_EngineAssetsPath;
			NCPath::Join(Settings::General::s_ImGuiConfigPath, NCPath::CreatePath("default.ini"));

			// Set the styles directory
			Settings::General::s_StylesDirectory = Settings::General::s_EngineAssetsPath;
			NCPath::Join(Settings::General::s_StylesDirectory, NCPath::CreatePath("styles"));

			// Create application store data if it does not exist
			CPath cocoaEngine = File::GetSpecialAppFolder();
			NCPath::Join(cocoaEngine, NCPath::CreatePath("CocoaEngine"));
			File::CreateDirIfNotExists(cocoaEngine);

			CPath editorSaveData = cocoaEngine;
			NCPath::Join(editorSaveData, Settings::General::s_EditorSaveData);
			Settings::General::s_EditorSaveData = editorSaveData;
			CPath editorStyleData = cocoaEngine;
			NCPath::Join(editorStyleData, Settings::General::s_EditorStyleData);
			Settings::General::s_EditorStyleData = editorStyleData;

			// Copy default script files to the assets path
			CPath defaultScriptH = Settings::General::s_EngineAssetsPath;
			NCPath::Join(defaultScriptH, NCPath::CreatePath("defaultCodeFiles"));
			NCPath::Join(defaultScriptH, NCPath::CreatePath("DefaultScript.h"));
			CPath defaultScriptCpp = Settings::General::s_EngineAssetsPath;
			NCPath::Join(defaultScriptCpp, NCPath::CreatePath("defaultCodeFiles"));
			NCPath::Join(defaultScriptCpp, NCPath::CreatePath("DefaultScript.cpp"));
			File::CopyFile(defaultScriptH, cocoaEngine, "DefaultScript");
			File::CopyFile(defaultScriptCpp, cocoaEngine, "DefaultScript");
		}

		bool CreateProject(SceneData& scene, const CPath& projectPath, const char* filename)
		{
			Settings::General::s_CurrentProject = projectPath;
			NCPath::Join(Settings::General::s_CurrentProject, NCPath::CreatePath(std::string(filename) + ".cprj"));
			Settings::General::s_CurrentScene = projectPath;
			NCPath::Join(Settings::General::s_CurrentScene, NCPath::CreatePath("scenes"));
			NCPath::Join(Settings::General::s_CurrentScene, NCPath::CreatePath("NewScene.cocoa"));
			Settings::General::s_WorkingDirectory = NCPath::CreatePath(NCPath::GetDirectory(Settings::General::s_CurrentProject, -1));

			json saveData = {
				{"ProjectPath", Settings::General::s_CurrentProject.Path.c_str()},
				{"CurrentScene", Settings::General::s_CurrentScene.Path.c_str()},
				{"WorkingDirectory", Settings::General::s_WorkingDirectory.Path.c_str() }
			};

			File::WriteFile(saveData.dump(4).c_str(), Settings::General::s_CurrentProject);
			CPath assetsPath = projectPath;
			NCPath::Join(assetsPath, NCPath::CreatePath("assets"));
			File::CreateDirIfNotExists(assetsPath);
			CPath scriptsPath = projectPath;
			NCPath::Join(scriptsPath, NCPath::CreatePath("scripts"));
			File::CreateDirIfNotExists(scriptsPath);
			CPath scenesPath = projectPath;
			NCPath::Join(scenesPath, NCPath::CreatePath("scenes"));
			File::CreateDirIfNotExists(scenesPath);

			CocoaEditor* application = (CocoaEditor*)Application::Get();
			Scene::FreeResources(scene);
			Scene::Init(scene);
			Scene::Save(scene, Settings::General::s_CurrentScene);
			m_SourceFileWatcher = std::make_shared<SourceFileWatcher>(scriptsPath);
			SaveEditorData();

			return true;
		}

		void SaveEditorData()
		{
			if (m_ProjectLoaded)
			{
				ImGui::SaveIniSettingsToDisk(Settings::General::s_ImGuiConfigPath.Path.c_str());
			}

			json saveData = {
				{"ProjectPath", Settings::General::s_CurrentProject.Path.c_str()},
				{"EditorStyle", Settings::General::s_EditorStyleData.Path.c_str()},
				{"ImGuiConfig", Settings::General::s_ImGuiConfigPath.Path.c_str()}
			};

			File::WriteFile(saveData.dump(4).c_str(), Settings::General::s_EditorSaveData);
		}

		bool LoadEditorData(SceneData& scene, const CPath& path)
		{
			FileHandle* editorData = File::OpenFile(path);
			if (editorData->m_Size > 0)
			{
				json j = json::parse(editorData->m_Data);
				if (!j["EditorStyle"].is_null())
				{
					Settings::General::s_EditorStyleData = NCPath::CreatePath(j["EditorStyle"], false);
				}

				if (!j["ImGuiConfigPath"].is_null())
				{
					Settings::General::s_ImGuiConfigPath = NCPath::CreatePath(j["ImGuiConfig"], false);
				}

				if (!j["ProjectPath"].is_null())
				{
					LoadProject(scene, NCPath::CreatePath(j["ProjectPath"], false));
				}
			}
			File::CloseFile(editorData);

			return true;
		}

		void SaveProject()
		{
			json saveData = {
				{"ProjectPath", Settings::General::s_CurrentProject.Path.c_str()},
				{"CurrentScene", Settings::General::s_CurrentScene.Path.c_str()},
				{"WorkingDirectory", NCPath::GetDirectory(Settings::General::s_CurrentProject, -1) }
			};

			File::WriteFile(saveData.dump(4).c_str(), Settings::General::s_CurrentProject);

			SaveEditorData();
		}

		bool LoadProject(SceneData& scene, const CPath& path)
		{
			Settings::General::s_CurrentProject = path;
			FileHandle* projectData = File::OpenFile(Settings::General::s_CurrentProject);
			bool isLoaded = false;
			if (projectData->m_Size > 0)
			{
				json j = json::parse(projectData->m_Data);
				if (!j["CurrentScene"].is_null())
				{
					Settings::General::s_CurrentScene = NCPath::CreatePath(j["CurrentScene"], false);
					Settings::General::s_WorkingDirectory = NCPath::CreatePath(j["WorkingDirectory"], false);

					CocoaEditor* application = (CocoaEditor*)Application::Get();
					Scene::Load(scene, Settings::General::s_CurrentScene);

					SaveEditorData();
					std::string winTitle = std::string(NCPath::Filename(Settings::General::s_CurrentProject)) + " -- " + std::string(NCPath::Filename(Settings::General::s_CurrentScene));
					Application::Get()->GetWindow()->SetTitle(winTitle.c_str());

					CPath scriptsPath = Settings::General::s_WorkingDirectory;
					NCPath::Join(scriptsPath, NCPath::CreatePath("scripts"));
					m_SourceFileWatcher = std::make_shared<SourceFileWatcher>(scriptsPath);
					static_cast<CocoaEditor*>(Application::Get())->SetProjectLoaded();
					isLoaded = true;
				}
			}

			File::CloseFile(projectData);
			return isLoaded;
		}

		void OnAttach(SceneData& scene)
		{
			LoadEditorData(scene, Settings::General::s_EditorSaveData);
		}

		void OnUpdate(SceneData& scene, float dt)
		{
			if (scene.IsPlaying)
			{
				m_EditorUpdate = false;
			}
			else
			{
				m_EditorUpdate = true;
			}

			if (CocoaEditor::IsProjectLoaded() && !m_EditorUpdate)
			{
				DebugDraw::BeginFrame();
				Scene::Update(scene, dt);
			}
			else if (CocoaEditor::IsProjectLoaded())
			{
				DebugDraw::BeginFrame();
				Scene::EditorUpdate(scene, dt);
				LevelEditorSystem::EditorUpdate(scene, dt);
				GizmoSystem::EditorUpdate(scene, dt);

				// TODO: It doesn't *really* matter where we put the imgui as long as it gets called... consider creating a dedicated imgui function for 
				// TODO: applications though...
				GizmoSystem::ImGui();
			}
		}

		void OnRender(SceneData& scene)
		{
			if (CocoaEditor::IsProjectLoaded())
			{
				Scene::Render(scene);
			}
		}

		void EditorLayer::OnEvent(SceneData& scene, Event& e)
		{
			if (CocoaEditor::IsProjectLoaded())
			{
				Scene::OnEvent(scene, e);
				// Order matters here. We want to do the top most layer to the bottom most so that events are blocked
				// if needed
				// TODO: Come up with better solution then if checks constantly. (Maybe abstract this into another function?)
				if (!e.Handled())
				{
					ImGuiLayer::OnEvent(scene, e);
				}
				if (!e.Handled())
				{
					LevelEditorSystem::OnEvent(scene, e);
				}
				if (!e.Handled())
				{
					GizmoSystem::OnEvent(scene, e);
				}
			}
		}

		void SetProjectLoaded()
		{
			m_ProjectLoaded = true;
		}

		bool IsProjectLoaded()
		{
			return m_ProjectLoaded; 
		}
	}

	// ===================================================================================
	// Editor Application
	// ===================================================================================
	CocoaEditor::CocoaEditor()
		: Application()
	{
	}

	void CocoaEditor::Init()
	{
		// This won't really do anything in release builds
		Cocoa::Memory::Init();

		// Initialize GLAD here, so that it works in DLL and exe
		Log::Info("Initializing GLAD functions in exe.");
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			Log::Error("Unable to initialize GLAD.");
		}

		// Engine initialization
		Cocoa::AssetManager::Init(0);
		Cocoa::ProjectWizard::Init();
		Cocoa::Input::Init();

		// Application Initialization
		Cocoa::EditorLayer::Init();
		Cocoa::ImGuiLayer::Init(GetWindow()->GetNativeWindow());

		m_CurrentScene = Scene::Create(new LevelEditorSceneInitializer());
		Scene::Init(m_CurrentScene);
		Scene::Start(m_CurrentScene);
		DebugDraw::Init();
	}

	void CocoaEditor::Shutdown()
	{
		// Engine shutdown sequence... Well maybe we'll add more here at another time
		// maybe not because at this point the OS should reclaim all resources. But this
		// would be a good place to put in saving anything that wasn't saved already
		// TODO: Try to save temporary files just in case the engine shutdown prematurely

#if _COCOA_DEBUG
		// In debug builds free all the memory to make sure there are no leaks
		DebugDraw::Destroy();
		Scene::FreeResources(m_CurrentScene);
#endif
		
		// This won't really do anything in release builds
		Cocoa::Memory::Destroy();
	}

	void CocoaEditor::BeginFrame()
	{
		ImGuiLayer::BeginFrame(m_CurrentScene);
	}

	void CocoaEditor::EndFrame()
	{
		ImGuiLayer::EndFrame();
	}

	void CocoaEditor::SetAppData(AppOnAttachFn attachFn, AppOnUpdateFn updateFn, AppOnRenderFn renderFn, AppOnEventFn eventFn)
	{
		m_AppData.AppOnAttach = attachFn;
		m_AppData.AppOnUpdate = updateFn;
		m_AppData.AppOnRender = renderFn;
		m_AppData.AppOnEvent = eventFn;
	}

	// ===================================================================================
	// Create application entry point
	// ===================================================================================
	Application* CreateApplication()
	{
		CocoaEditor* editor = new CocoaEditor();
		editor->SetAppData(
			EditorLayer::OnAttach,
			EditorLayer::OnUpdate,
			EditorLayer::OnRender,
			EditorLayer::OnEvent
		);
		return editor;
	}
}