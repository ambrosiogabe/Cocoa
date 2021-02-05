#pragma once
#include "gui/ImGuiExtended.h"
#include "gui/FontAwesome.h"
#include "core/CocoaEditorApplication.h"

#include "cocoa/core/CWindow.h"
#include "cocoa/core/Application.h"
#include "cocoa/file/IFile.h"
#include "cocoa/file/IFileDialog.h"

namespace Cocoa
{
	Texture ProjectWizard::s_JadeLogo = {};
	glm::vec2 ProjectWizard::s_IdealSize{ 0, 0 };
	glm::vec2 ProjectWizard::s_TexturePos{ 0, 0 };
	glm::vec2 ProjectWizard::s_VersionPos{ 0, 0 };
	glm::vec2 ProjectWizard::s_CreateProjButtonPos{ 0, 0 };
	glm::vec2 ProjectWizard::s_OpenProjectButtonPos{ 0, 0 };
	glm::vec2 ProjectWizard::s_ButtonSize{ 0, 0 };
	glm::vec2 ProjectWizard::s_Padding{ 10.0f, 20.0f };

	bool ProjectWizard::s_CreatingProject = false;
	char ProjectWizard::s_TmpFilename[256];
	CPath ProjectWizard::s_NewProjectPath = "";

	void ProjectWizard::Init()
	{
		s_TmpFilename[0] = '\0';
		s_IdealSize = Application::Get()->GetWindow()->GetMonitorSize() / 2.0f;

		s_JadeLogo.MagFilter = FilterMode::Linear;
		s_JadeLogo.MinFilter = FilterMode::Linear;
		s_JadeLogo.WrapS = WrapMode::Repeat;
		s_JadeLogo.WrapT = WrapMode::Repeat;
		TextureUtil::Generate(s_JadeLogo, CPath("assets/jadeLogo.png"));
		
		s_TexturePos.x = (s_IdealSize.x / 2.0f) - (s_JadeLogo.Width / 2.0f);
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
			s_VersionPos.y = s_TexturePos.y + s_JadeLogo.Height + textSize.y / 2.0f;
		}

		Application::Get()->GetWindow()->SetSize(s_IdealSize);
		glm::vec2 winPos = Application::Get()->GetWindow()->GetWindowPos();
		ImGui::SetNextWindowPos(ImVec2(winPos.x, winPos.y));
		ImGui::SetNextWindowSize(ImVec2(s_IdealSize.x, s_IdealSize.y), ImGuiCond_Once);
		ImGui::Begin("Create or Open Project", &open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

		ImGui::SetCursorPos(ImVec2(s_TexturePos.x, s_TexturePos.y));
		ImGui::Image(reinterpret_cast<void*>(s_JadeLogo.GraphicsId), ImVec2(s_JadeLogo.Width, s_JadeLogo.Height));
		ImGui::SetCursorPos(ImVec2(s_VersionPos.x, s_VersionPos.y));
		ImGui::Text("Version 1.0 Alpha");

		ImGui::SetCursorPos(ImVec2(s_CreateProjButtonPos.x, s_CreateProjButtonPos.y));
		if (CImGui::Button(ICON_FA_PLUS " Create Project", s_ButtonSize))
		{
			s_CreatingProject = true;
		}

		ImGui::SetCursorPos(ImVec2(s_OpenProjectButtonPos.x, s_OpenProjectButtonPos.y));
		if (CImGui::Button(ICON_FA_FOLDER_OPEN " Open Project", s_ButtonSize) && !s_CreatingProject)
		{
			FileDialogResult res;
			if (IFileDialog::GetOpenFileName("", res, { {"Cocoa Project", "*.cprj"} }))
			{
				if (!EditorLayer::LoadProject(CPath(res.filepath)))
				{
					Log::Warning("Unable to load project: %s", res.filepath.c_str());
				}
			}
		}

		ImGui::End();

		if (s_CreatingProject)
		{
			CreateProjectImGui(s_CreatingProject);
		}
	}


	void ProjectWizard::CreateProjectImGui(bool& windowOpen)
	{
		ImGui::Begin("Create New Project", &windowOpen);

		ImGui::LabelText("##tmp_projectname", "Project Name:");
		ImGui::InputText("##tmp_filename", s_TmpFilename, 256);
		ImGui::LabelText("##tmp_projectdir", "Project Directory:");
		ImGui::LabelText("##tmp_showfile", "%s", s_NewProjectPath.Filepath());
		ImGui::SameLine();

		if (CImGui::Button("Choose Directory"))
		{
			FileDialogResult res;
			if (IFileDialog::GetOpenFolderName(".", res))
			{
				s_NewProjectPath = res.filepath;
			}
		}

		if (CImGui::Button("Cancel"))
		{
			windowOpen = false;
		}
		ImGui::SameLine();
		if (CImGui::Button("Create"))
		{
			if (EditorLayer::CreateProject(s_NewProjectPath, s_TmpFilename))
			{
				CocoaEditor* e = static_cast<CocoaEditor*>(Application::Get());
				e->SetProjectLoaded();
				windowOpen = false;
			}
		}
		ImGui::End();
	}
}