#pragma once
#include "Gui/ImGuiExtended.h"
#include "JadeEditorApplication.h"
#include "FontAwesome.h"

#include "jade/core/JWindow.h"
#include "jade/core/Application.h"
#include "jade/file/IFile.h"
#include "jade/file/IFileDialog.h"

namespace Jade
{
	Texture* ProjectWizard::s_JadeLogo = nullptr;
	glm::vec2 ProjectWizard::s_IdealSize{ 0, 0 };
	glm::vec2 ProjectWizard::s_TexturePos{ 0, 0 };
	glm::vec2 ProjectWizard::s_VersionPos{ 0, 0 };
	glm::vec2 ProjectWizard::s_CreateProjButtonPos{ 0, 0 };
	glm::vec2 ProjectWizard::s_OpenProjectButtonPos{ 0, 0 };
	glm::vec2 ProjectWizard::s_ButtonSize{ 0, 0 };
	glm::vec2 ProjectWizard::s_Padding{ 10.0f, 20.0f };

	bool ProjectWizard::s_CreatingProject = false;
	char ProjectWizard::s_TmpFilename[256];
	JPath ProjectWizard::s_NewProjectPath = "";

	void ProjectWizard::Init()
	{
		s_TmpFilename[0] = '\0';
		s_IdealSize = Application::Get()->GetWindow()->GetMonitorSize() / 2.0f;
		s_JadeLogo = new Texture(JPath("assets/jadeLogo.png"));
		s_JadeLogo->Load();
		s_TexturePos.x = (s_IdealSize.x / 2.0f) - (s_JadeLogo->GetWidth() / 2.0f);
		s_TexturePos.y = s_IdealSize.y / 10.0f;
		s_VersionPos = glm::vec2(-1.0f, -1.0f);
		s_ButtonSize = glm::vec2(s_IdealSize.x / 3.0f, s_IdealSize.y / 18.0f);
		s_CreateProjButtonPos = (s_IdealSize / 2.0f) - (s_ButtonSize / 2.0f);
		s_OpenProjectButtonPos = s_CreateProjButtonPos + glm::vec2(0.0f, s_ButtonSize.y) + glm::vec2(0.0f, s_Padding.y);
	}

	void ProjectWizard::ImGui()
	{
		static bool open = true;
		if (s_VersionPos.x < 0 && s_VersionPos.y < 0)
		{
			ImVec2 textSize = ImGui::CalcTextSize("Version 1.0 Alpha");
			s_VersionPos = (s_IdealSize / 2.0f) - glm::vec2(textSize.x / 2.0f, textSize.y / 2.0f);
			s_VersionPos.y = s_TexturePos.y + s_JadeLogo->GetHeight() + textSize.y / 2.0f;
		}

		Application::Get()->GetWindow()->SetSize(s_IdealSize);
		glm::vec2 winPos = Application::Get()->GetWindow()->GetWindowPos();
		ImGui::SetNextWindowPos(ImVec2(winPos.x, winPos.y));
		ImGui::SetNextWindowSize(ImVec2(s_IdealSize.x, s_IdealSize.y), ImGuiCond_Once);
		ImGui::Begin("Create or Open Project", &open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

		ImGui::SetCursorPos(ImVec2(s_TexturePos.x, s_TexturePos.y));
		ImGui::Image(reinterpret_cast<void*>(s_JadeLogo->GetId()), ImVec2(s_JadeLogo->GetWidth(), s_JadeLogo->GetHeight()));
		ImGui::SetCursorPos(ImVec2(s_VersionPos.x, s_VersionPos.y));
		ImGui::Text("Version 1.0 Alpha");

		ImGui::SetCursorPos(ImVec2(s_CreateProjButtonPos.x, s_CreateProjButtonPos.y));
		if (JImGui::Button(ICON_FA_PLUS " Create Project", s_ButtonSize))
		{
			s_CreatingProject = true;
		}

		ImGui::SetCursorPos(ImVec2(s_OpenProjectButtonPos.x, s_OpenProjectButtonPos.y));
		if (JImGui::Button(ICON_FA_FOLDER_OPEN " Open Project", s_ButtonSize) && !s_CreatingProject)
		{
			FileDialogResult res;
			if (IFileDialog::GetOpenFileName("", res, { {"Jade Project", "*.jprj"} }))
			{
				if (!EditorLayer::LoadProject(JPath(res.filepath)))
				{
					Log::Warning("Unable to load project: %s", res.filepath.c_str());
				}
			}
		}

		ImGui::End();

		if (s_CreatingProject)
		{
			if (CreateProjectImGui())
			{
				s_CreatingProject = false;
			}
		}
	}


	bool ProjectWizard::CreateProjectImGui()
	{
		bool res = false;

		ImGui::Begin("Create New Project");

		ImGui::LabelText("##tmp_projectname", "Project Name:");
		ImGui::InputText("##tmp_filename", s_TmpFilename, 256);
		ImGui::LabelText("##tmp_projectdir", "Project Directory:");
		ImGui::LabelText("##tmp_showfile", "%s", s_NewProjectPath.Filepath());
		ImGui::SameLine();

		if (JImGui::Button("Choose Directory"))
		{
			FileDialogResult res;
			if (IFileDialog::GetOpenFolderName(".", res))
			{
				s_NewProjectPath = res.filepath;
			}
		}

		if (JImGui::Button("Cancel"))
		{
			res = true;
		}
		ImGui::SameLine();
		if (JImGui::Button("Create"))
		{
			if (EditorLayer::CreateProject(s_NewProjectPath, s_TmpFilename))
			{
				JadeEditor* e = static_cast<JadeEditor*>(Application::Get());
				e->SetProjectLoaded();
				res = true;
			}
		}
		ImGui::End();

		return res;
	}
}