#include "Cocoa.h"

#include "core/CocoaEditorApplication.h"
#include "core/LevelEditorSceneInitializer.h"
#include "core/ImGuiLayer.h"
#include "gui/ImGuiHeader.h"
#include "editorWindows/InspectorWindow.h"
#include "nativeScripting/SourceFileWatcher.h"
#include "renderer/Gizmos.h"

#include "cocoa/file/IFile.h"
#include "cocoa/util/Settings.h"
#include "cocoa/systems/RenderSystem.h"

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
			Settings::General::s_EngineExeDirectory = NCPath::CreatePath(NCPath::GetDirectory(IFile::GetExecutableDirectory(), -1));
			Settings::General::s_EngineSourceDirectory = NCPath::CreatePath(NCPath::GetDirectory(IFile::GetExecutableDirectory(), -4));
			Log::Info("%s", Settings::General::s_EngineExeDirectory.Path.c_str());
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

			IFile::WriteFile(saveData.dump(4).c_str(), Settings::General::s_CurrentProject);
			CPath assetsPath = projectPath;
			NCPath::Join(assetsPath, NCPath::CreatePath("assets"));
			IFile::CreateDirIfNotExists(assetsPath);
			CPath scriptsPath = projectPath;
			NCPath::Join(scriptsPath, NCPath::CreatePath("scripts"));
			IFile::CreateDirIfNotExists(scriptsPath);
			CPath scenesPath = projectPath;
			NCPath::Join(scenesPath, NCPath::CreatePath("scenes"));
			IFile::CreateDirIfNotExists(scenesPath);

			CocoaEditor* application = (CocoaEditor*)Application::Get();
			Scene::Reset(scene);
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

			IFile::WriteFile(saveData.dump(4).c_str(), Settings::General::s_EditorSaveData);
		}

		bool LoadEditorData(SceneData& scene, const CPath& path)
		{
			File* editorData = IFile::OpenFile(path);
			if (editorData->m_Data.size() > 0)
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
			IFile::CloseFile(editorData);

			return true;
		}

		void SaveProject()
		{
			json saveData = {
				{"ProjectPath", Settings::General::s_CurrentProject.Path.c_str()},
				{"CurrentScene", Settings::General::s_CurrentScene.Path.c_str()},
				{"WorkingDirectory", NCPath::GetDirectory(Settings::General::s_CurrentProject, -1) }
			};

			IFile::WriteFile(saveData.dump(4).c_str(), Settings::General::s_CurrentProject);

			SaveEditorData();
		}

		bool LoadProject(SceneData& scene, const CPath& path)
		{
			Settings::General::s_CurrentProject = path;
			File* projectData = IFile::OpenFile(Settings::General::s_CurrentProject);
			if (projectData->m_Data.size() > 0)
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
					return true;
				}
			}

			return false;
		}

		void OnAttach(SceneData& scene)
		{
			// Set the assets path as CWD (which should be where the exe is currently located)
			Settings::General::s_EngineAssetsPath = IFile::GetCwd();
			NCPath::Join(Settings::General::s_EngineAssetsPath, NCPath::CreatePath("assets"));
			Settings::General::s_ImGuiConfigPath = Settings::General::s_EngineAssetsPath;
			NCPath::Join(Settings::General::s_ImGuiConfigPath, NCPath::CreatePath("default.ini"));

			// Create application store data if it does not exist
			CPath cocoaEngine = IFile::GetSpecialAppFolder();
			NCPath::Join(cocoaEngine, NCPath::CreatePath("CocoaEngine"));
			IFile::CreateDirIfNotExists(cocoaEngine);

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
			IFile::CopyFile(defaultScriptH, cocoaEngine, "DefaultScript");
			IFile::CopyFile(defaultScriptCpp, cocoaEngine, "DefaultScript");

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
		// Initialize GLAD here, so that it works in DLL and exe
		Log::Info("Initializing GLAD functions in exe.");
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			Log::Error("Unable to initialize GLAD.");
		}

		// Engine initialization
		Cocoa::AssetManager::Init(0);
		Cocoa::IFile::Init();
		Cocoa::ProjectWizard::Init();
		Cocoa::Input::Init();

		// Application Initialization
		Cocoa::ImGuiLayer::Init(GetWindow()->GetNativeWindow());
		Cocoa::EditorLayer::Init();

		ChangeScene(new LevelEditorSceneInitializer());
	}

	void CocoaEditor::Shutdown()
	{
		// Engine shutdown sequence
		Cocoa::IFile::Destroy();
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