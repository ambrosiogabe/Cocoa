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
			Settings::General::s_EngineExeDirectory = IFile::GetExecutableDirectory().GetDirectory(-1);
			Settings::General::s_EngineSourceDirectory = IFile::GetExecutableDirectory().GetDirectory(-4);
			Log::Info("%s", Settings::General::s_EngineExeDirectory.Filepath());
		}

		bool CreateProject(Scene* scene, const CPath& projectPath, const char* filename)
		{
			Settings::General::s_CurrentProject = projectPath + (std::string(filename) + ".cprj");
			Settings::General::s_CurrentScene = projectPath + "scenes" + "NewScene.cocoa";
			Settings::General::s_WorkingDirectory = Settings::General::s_CurrentProject.GetDirectory(-1);

			json saveData = {
				{"ProjectPath", Settings::General::s_CurrentProject.Filepath()},
				{"CurrentScene", Settings::General::s_CurrentScene.Filepath()},
				{"WorkingDirectory", Settings::General::s_WorkingDirectory.Filepath() }
			};

			IFile::WriteFile(saveData.dump(4).c_str(), Settings::General::s_CurrentProject);
			IFile::CreateDirIfNotExists(projectPath + "assets");
			IFile::CreateDirIfNotExists(projectPath + "scripts");
			IFile::CreateDirIfNotExists(projectPath + "scenes");

			CocoaEditor* application = (CocoaEditor*)Application::Get();
			scene->Reset();
			scene->Save(Settings::General::s_CurrentScene);
			m_SourceFileWatcher = std::make_shared<SourceFileWatcher>(projectPath + "scripts");
			SaveEditorData();

			return true;
		}

		void SaveEditorData()
		{
			if (m_ProjectLoaded)
			{
				ImGui::SaveIniSettingsToDisk(Settings::General::s_ImGuiConfigPath.Filepath());
			}

			json saveData = {
				{"ProjectPath", Settings::General::s_CurrentProject.Filepath()},
				{"EditorStyle", Settings::General::s_EditorStyleData.Filepath()},
				{"ImGuiConfig", Settings::General::s_ImGuiConfigPath.Filepath()}
			};

			IFile::WriteFile(saveData.dump(4).c_str(), Settings::General::s_EditorSaveData);
		}

		bool LoadEditorData(Scene* scene, const CPath& path)
		{
			File* editorData = IFile::OpenFile(path);
			if (editorData->m_Data.size() > 0)
			{
				json j = json::parse(editorData->m_Data);
				if (!j["EditorStyle"].is_null())
				{
					Settings::General::s_EditorStyleData = CPath(j["EditorStyle"], false);
				}

				if (!j["ImGuiConfigPath"].is_null())
				{
					Settings::General::s_ImGuiConfigPath = CPath(j["ImGuiConfig"], false);
				}

				if (!j["ProjectPath"].is_null())
				{
					LoadProject(scene, CPath(j["ProjectPath"], false));
				}
			}
			IFile::CloseFile(editorData);

			return true;
		}

		void SaveProject()
		{
			json saveData = {
				{"ProjectPath", Settings::General::s_CurrentProject.Filepath()},
				{"CurrentScene", Settings::General::s_CurrentScene.Filepath()},
				{"WorkingDirectory", Settings::General::s_CurrentProject.GetDirectory(-1) }
			};

			IFile::WriteFile(saveData.dump(4).c_str(), Settings::General::s_CurrentProject);

			SaveEditorData();
		}

		bool LoadProject(Scene* scene, const CPath& path)
		{
			Settings::General::s_CurrentProject = path;
			File* projectData = IFile::OpenFile(Settings::General::s_CurrentProject);
			if (projectData->m_Data.size() > 0)
			{
				json j = json::parse(projectData->m_Data);
				if (!j["CurrentScene"].is_null())
				{
					Settings::General::s_CurrentScene = CPath(j["CurrentScene"], false);
					Settings::General::s_WorkingDirectory = CPath(j["WorkingDirectory"], false);

					CocoaEditor* application = (CocoaEditor*)Application::Get();
					scene->Load(Settings::General::s_CurrentScene);

					SaveEditorData();
					std::string winTitle = std::string(Settings::General::s_CurrentProject.Filename()) + " -- " + std::string(Settings::General::s_CurrentScene.Filename());
					Application::Get()->GetWindow()->SetTitle(winTitle.c_str());

					m_SourceFileWatcher = std::make_shared<SourceFileWatcher>(Settings::General::s_WorkingDirectory + "scripts");
					static_cast<CocoaEditor*>(Application::Get())->SetProjectLoaded();
					return true;
				}
			}

			return false;
		}

		void OnAttach(Scene* scene)
		{
			// Set the assets path as CWD (which should be where the exe is currently located)
			Settings::General::s_EngineAssetsPath = IFile::GetCwd() + Settings::General::s_EngineAssetsPath;
			Settings::General::s_ImGuiConfigPath = Settings::General::s_EngineAssetsPath + Settings::General::s_ImGuiConfigPath;

			// Create application store data if it does not exist
			IFile::CreateDirIfNotExists(IFile::GetSpecialAppFolder() + "CocoaEngine");

			Settings::General::s_EditorSaveData = IFile::GetSpecialAppFolder() + "CocoaEngine" + Settings::General::s_EditorSaveData;
			Settings::General::s_EditorStyleData = IFile::GetSpecialAppFolder() + "CocoaEngine" + Settings::General::s_EditorStyleData;

			// Copy default script files to the assets path
			CPath scriptHFile = IFile::GetSpecialAppFolder() + "CocoaEngine" + "DefaultScript.h";
			CPath scriptCppFile = IFile::GetSpecialAppFolder() + "CocoaEngine" + "DefaultScript.cpp";
			IFile::CopyFile(Settings::General::s_EngineAssetsPath + "defaultCodeFiles" + "DefaultScript.h", IFile::GetSpecialAppFolder() + "CocoaEngine", "DefaultScript");
			IFile::CopyFile(Settings::General::s_EngineAssetsPath + "defaultCodeFiles" + "DefaultScript.cpp", IFile::GetSpecialAppFolder() + "CocoaEngine", "DefaultScript");

			LoadEditorData(scene, Settings::General::s_EditorSaveData);
		}

		void OnUpdate(Scene* scene, float dt)
		{
			if (scene->IsPlaying())
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
				scene->Update(dt);
			}
			else if (CocoaEditor::IsProjectLoaded())
			{
				DebugDraw::BeginFrame();
				scene->EditorUpdate(dt);
				LevelEditorSystem::EditorUpdate(scene, dt);
				GizmoSystem::EditorUpdate(scene, dt);

				// TODO: It doesn't *really* matter where we put the imgui as long as it gets called... consider creating a dedicated imgui function for 
				// TODO: applications though...
				GizmoSystem::ImGui();
			}
		}

		void OnRender(Scene* scene)
		{
			if (CocoaEditor::IsProjectLoaded())
			{
				scene->Render();
			}
		}

		void EditorLayer::OnEvent(Scene* scene, Event& e)
		{
			if (CocoaEditor::IsProjectLoaded())
			{
				const auto& systems = scene->GetSystems();

				for (const auto& system : systems)
				{
					system->OnEvent(e);
				}

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
		Cocoa::IFileDialog::Init();
		Cocoa::IFile::Init();
		Cocoa::ProjectWizard::Init();
		Cocoa::Physics2D::Init();
		Cocoa::Input::Init();
		Cocoa::RenderSystem::Init();

		// Application Initialization
		Cocoa::ImGuiLayer::Init(GetWindow()->GetNativeWindow());
		Cocoa::EditorLayer::Init();

		ChangeScene(new LevelEditorSceneInitializer());
	}

	void CocoaEditor::Shutdown()
	{
		// Engine shutdown sequence
		Cocoa::IFileDialog::Destroy();
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