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
	namespace ProjectWizard
	{
		// Internal Variables
		static Texture mJadeLogo = {};
		static glm::vec2 mIdealSize{ 0, 0 };
		static glm::vec2 mTexturePos{ 0, 0 };
		static glm::vec2 mVersionPos{ 0, 0 };
		static glm::vec2 mCreateProjectButtonPos{ 0, 0 };
		static glm::vec2 mOpenProjectButtonPos{ 0, 0 };
		static glm::vec2 mButtonSize{ 0, 0 };
		static glm::vec2 mPadding{ 10.0f, 20.0f };
		
		static bool mCreatingProject = false;
		static char mTmpFilename[256];
		static CPath mNewProjectPath = NCPath::CreatePath();

		void Init()
		{
			mTmpFilename[0] = '\0';
			mIdealSize = Application::Get()->GetWindow()->GetMonitorSize() / 2.0f;

			mJadeLogo.MagFilter = FilterMode::Linear;
			mJadeLogo.MinFilter = FilterMode::Linear;
			mJadeLogo.WrapS = WrapMode::Repeat;
			mJadeLogo.WrapT = WrapMode::Repeat;
			TextureUtil::Generate(mJadeLogo, NCPath::CreatePath("assets/jadeLogo.png"));

			mTexturePos.x = (mIdealSize.x / 2.0f) - (mJadeLogo.Width / 2.0f);
			mTexturePos.y = mIdealSize.y / 10.0f;
			mVersionPos = glm::vec2(-1.0f, -1.0f);
			mButtonSize = glm::vec2(mIdealSize.x / 3.0f, mIdealSize.y / 18.0f);
			mCreateProjectButtonPos = (mIdealSize / 2.0f) - (mButtonSize / 2.0f);
			mOpenProjectButtonPos = mCreateProjectButtonPos + glm::vec2(0.0f, mButtonSize.y) + glm::vec2(0.0f, mPadding.y);
		}

		void ImGui(SceneData& scene)
		{
			static bool open = true;
			if (mVersionPos.x < 0 && mVersionPos.y < 0)
			{
				ImVec2 textSize = ImGui::CalcTextSize("Version 1.0 Alpha");
				mVersionPos = (mIdealSize / 2.0f) - glm::vec2(textSize.x / 2.0f, textSize.y / 2.0f);
				mVersionPos.y = mTexturePos.y + mJadeLogo.Height + textSize.y / 2.0f;
			}

			Application::Get()->GetWindow()->SetSize(mIdealSize);
			glm::vec2 winPos = Application::Get()->GetWindow()->GetWindowPos();
			ImGui::SetNextWindowPos(ImVec2(winPos.x, winPos.y));
			ImGui::SetNextWindowSize(ImVec2(mIdealSize.x, mIdealSize.y), ImGuiCond_Once);
			ImGui::Begin("Create or Open Project", &open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

			ImGui::SetCursorPos(ImVec2(mTexturePos.x, mTexturePos.y));
			ImGui::Image((void*)(mJadeLogo.GraphicsId), ImVec2((float)mJadeLogo.Width, (float)mJadeLogo.Height));
			ImGui::SetCursorPos(ImVec2(mVersionPos.x, mVersionPos.y));
			ImGui::Text("Version 1.0 Alpha");

			ImGui::SetCursorPos(ImVec2(mCreateProjectButtonPos.x, mCreateProjectButtonPos.y));
			if (CImGui::Button(ICON_FA_PLUS " Create Project", mButtonSize))
			{
				mCreatingProject = true;
			}

			ImGui::SetCursorPos(ImVec2(mOpenProjectButtonPos.x, mOpenProjectButtonPos.y));
			if (CImGui::Button(ICON_FA_FOLDER_OPEN " Open Project", mButtonSize) && !mCreatingProject)
			{
				FileDialogResult res;
				if (IFileDialog::GetOpenFileName("", res, { {"Cocoa Project", "*.cprj"} }))
				{
					if (!EditorLayer::LoadProject(scene, NCPath::CreatePath(res.filepath)))
					{
						Log::Warning("Unable to load project: %s", res.filepath.c_str());
					}
				}
			}

			ImGui::End();

			if (mCreatingProject)
			{
				CreateProjectImGui(scene, mCreatingProject);
			}
		}


		void CreateProjectImGui(SceneData& scene, bool& windowOpen)
		{
			ImGui::Begin("Create New Project", &windowOpen);

			ImGui::LabelText("##tmp_projectname", "Project Name:");
			ImGui::InputText("##tmp_filename", mTmpFilename, 256);
			ImGui::LabelText("##tmp_projectdir", "Project Directory:");
			ImGui::LabelText("##tmp_showfile", "%s", mNewProjectPath.Path.c_str());
			ImGui::SameLine();

			if (CImGui::Button("Choose Directory"))
			{
				FileDialogResult res;
				if (IFileDialog::GetOpenFolderName(".", res))
				{
					mNewProjectPath = NCPath::CreatePath(res.filepath);
				}
			}

			if (CImGui::Button("Cancel"))
			{
				windowOpen = false;
			}
			ImGui::SameLine();
			if (CImGui::Button("Create"))
			{
				if (EditorLayer::CreateProject(scene, mNewProjectPath, mTmpFilename))
				{
					CocoaEditor* e = static_cast<CocoaEditor*>(Application::Get());
					e->SetProjectLoaded();
					windowOpen = false;
				}
			}
			ImGui::End();
		}
	}
}