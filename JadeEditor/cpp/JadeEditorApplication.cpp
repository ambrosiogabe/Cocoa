#include "Jade.h"

#include "JadeEditorApplication.h"
#include "LevelEditorScene.h"
#include "jade/file/IFile.h"
#include "jade/util/Settings.h"

#include <glad/glad.h>
#include <jsonVendor/json.hpp>

namespace Jade
{
	void EditorLayer::SaveEditorData()
	{
		ImGui::SaveIniSettingsToDisk(Settings::General::s_ImGuiConfigPath.Filepath());

		json saveData = {
			{"ProjectPath", Settings::General::s_CurrentProject.Filepath()},
			{"EditorStyle", Settings::General::s_EditorStyleData.Filepath()},
			{"ImGuiConfig", Settings::General::s_ImGuiConfigPath.Filepath()}
		};

		IFile::WriteFile(saveData.dump(4).c_str(), Settings::General::s_EditorSaveData);
	}

	bool EditorLayer::LoadEditorData(const JPath& path)
	{
		File* editorData = IFile::OpenFile(path);
		if (editorData->m_Data.size() > 0)
		{
			json j = json::parse(editorData->m_Data);
			if (!j["ProjectPath"].is_null())
			{
				if (LoadProject(JPath(j["ProjectPath"])))
				{
					// Set the project as loaded since we successfully loaded it
					JadeEditor::SetProjectLoaded();
				}
			}

			if (!j["EditorStyle"].is_null())
			{
				Settings::General::s_EditorStyleData = JPath(j["EditorStyle"]);
			}

			if (!j["ImGuiConfigPath"].is_null())
			{
				Settings::General::s_ImGuiConfigPath = JPath(j["ImGuiConfig"]);
			}
		}
		IFile::CloseFile(editorData);

		return true;
	}

	void EditorLayer::SaveProject()
	{
		json saveData = {
			{"ProjectPath", Settings::General::s_CurrentProject.Filepath()},
			{"CurrentScene", Settings::General::s_CurrentScene.Filepath()},
			{"WorkingDirectory", Settings::General::s_CurrentProject.GetDirectory(-1) }
		};

		IFile::WriteFile(saveData.dump(4).c_str(), Settings::General::s_CurrentProject);

		SaveEditorData();
	}

	bool EditorLayer::LoadProject(const JPath& path)
	{
		Settings::General::s_CurrentProject = path;
		File* projectData = IFile::OpenFile(Settings::General::s_CurrentProject);
		if (projectData->m_Data.size() > 0)
		{
			json j = json::parse(projectData->m_Data);
			if (!j["CurrentScene"].is_null())
			{
				Settings::General::s_CurrentScene = JPath(j["CurrentScene"]);
				Settings::General::s_WorkingDirectory = JPath(j["WorkingDirectory"]);
				Application::Get()->GetScene()->Load(JPath(j["CurrentScene"]));
				static_cast<JadeEditor*>(Application::Get())->SetProjectLoaded();
				return true;
			}
		}

		return false;
	}

	void EditorLayer::OnAttach()
	{
		// Set the assets path as CWD (which should be where the exe is currently located)
		Settings::General::s_EngineAssetsPath = IFile::GetCwd() + Settings::General::s_EngineAssetsPath;
		Settings::General::s_ImGuiConfigPath = Settings::General::s_EngineAssetsPath + Settings::General::s_ImGuiConfigPath;

		// Start the scene
		Application::Get()->ChangeScene(new LevelEditorScene());

		// Create application store data if it does not exist
		IFile::CreateDirIfNotExists(IFile::GetSpecialAppFolder() + "JadeEngine");

		Settings::General::s_EditorSaveData = IFile::GetSpecialAppFolder() + "JadeEngine" + Settings::General::s_EditorSaveData;
		Settings::General::s_EditorStyleData = IFile::GetSpecialAppFolder() + "JadeEngine" + Settings::General::s_EditorStyleData;

		LoadEditorData(Settings::General::s_EditorSaveData);
	}

	void EditorLayer::OnUpdate(float dt)
	{
		if (JadeEditor::IsProjectLoaded())
		{
			DebugDraw::BeginFrame();
			Application::Get()->GetScene()->Update(dt);
		}
		else
		{
			m_ProjectWizard.Update(dt);
		}
	}

	void EditorLayer::OnImGuiRender()
	{
		if (JadeEditor::IsProjectLoaded())
		{
			Application::Get()->GetScene()->ImGui();
		}
		else
		{
			m_ProjectWizard.ImGui();
		}
	}

	void EditorLayer::OnRender()
	{
		if (JadeEditor::IsProjectLoaded())
		{
			glBindFramebuffer(GL_FRAMEBUFFER, Application::Get()->GetFramebuffer()->GetId());
			glViewport(0, 0, 3840, 2160);
			glClearColor(0.45f, 0.55f, 0.6f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			DebugDraw::DrawBottomBatches();
			Application::Get()->GetScene()->Render();
			DebugDraw::DrawTopBatches();
		}
	}

	void EditorLayer::OnEvent(Event& e)
	{
		if (JadeEditor::IsProjectLoaded())
		{
			const auto& systems = Application::Get()->GetScene()->GetSystems();

			for (const auto& system : systems)
			{
				system->OnEvent(e);
			}
		}
	}

	// Create application entry point
	Application* CreateApplication()
	{
		JadeEditor* editor = new JadeEditor();
		return editor;
	}
}