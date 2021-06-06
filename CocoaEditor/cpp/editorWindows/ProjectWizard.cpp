#include "editorWindows/ProjectWizard.h"
#include "gui/ImGuiExtended.h"
#include "gui/FontAwesome.h"
#include "core/CocoaEditorApplication.h"

#include "cocoa/core/CWindow.h"
#include "cocoa/core/Application.h"
#include "cocoa/file/FileDialog.h"

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
		static Path mNewProjectPath = Path::createDefault();

		void init()
		{
			mTmpFilename[0] = '\0';
			mIdealSize = Application::get()->getWindow()->getMonitorSize() / 2.0f;

			mJadeLogo.magFilter = FilterMode::Linear;
			mJadeLogo.minFilter = FilterMode::Linear;
			mJadeLogo.wrapS = WrapMode::Repeat;
			mJadeLogo.wrapT = WrapMode::Repeat;
			TextureUtil::generate(mJadeLogo, PathBuilder("assets/jadeLogo.png").createTmpPath());

			mTexturePos.x = (mIdealSize.x / 2.0f) - (mJadeLogo.width / 2.0f);
			mTexturePos.y = mIdealSize.y / 10.0f;
			mVersionPos = glm::vec2(-1.0f, -1.0f);
			mButtonSize = glm::vec2(mIdealSize.x / 3.0f, mIdealSize.y / 18.0f);
			mCreateProjectButtonPos = (mIdealSize / 2.0f) - (mButtonSize / 2.0f);
			mOpenProjectButtonPos = mCreateProjectButtonPos + glm::vec2(0.0f, mButtonSize.y) + glm::vec2(0.0f, mPadding.y);
		}

		void imgui(SceneData& scene)
		{
			static bool open = true;
			if (mVersionPos.x < 0 && mVersionPos.y < 0)
			{
				ImVec2 textSize = ImGui::CalcTextSize("Version 1.0 Alpha");
				mVersionPos = (mIdealSize / 2.0f) - glm::vec2(textSize.x / 2.0f, textSize.y / 2.0f);
				mVersionPos.y = mTexturePos.y + mJadeLogo.height + textSize.y / 2.0f;
			}

			Application::get()->getWindow()->setSize(mIdealSize);
			glm::vec2 winPos = Application::get()->getWindow()->getWindowPos();
			ImGui::SetNextWindowPos(ImVec2(winPos.x, winPos.y));
			ImGui::SetNextWindowSize(ImVec2(mIdealSize.x, mIdealSize.y), ImGuiCond_Once);
			ImGui::Begin("Create or Open Project", &open, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

			ImGui::SetCursorPos(ImVec2(mTexturePos.x, mTexturePos.y));
			ImGui::Image((void*)(mJadeLogo.graphicsId), ImVec2((float)mJadeLogo.width, (float)mJadeLogo.height));
			ImGui::SetCursorPos(ImVec2(mVersionPos.x, mVersionPos.y));
			ImGui::Text("Version 1.0 Alpha");

			ImGui::SetCursorPos(ImVec2(mCreateProjectButtonPos.x, mCreateProjectButtonPos.y));
			if (CImGui::button(ICON_FA_PLUS " Create Project", mButtonSize))
			{
				mCreatingProject = true;
			}

			ImGui::SetCursorPos(ImVec2(mOpenProjectButtonPos.x, mOpenProjectButtonPos.y));
			if (CImGui::button(ICON_FA_FOLDER_OPEN " Open Project", mButtonSize) && !mCreatingProject)
			{
				FileDialogResult res;
				if (FileDialog::getOpenFileName("", res, { {"Cocoa Project", "*.cprj"} }))
				{
					if (!EditorLayer::loadProject(scene, PathBuilder(res.filepath.c_str()).createTmpPath()))
					{
						Logger::Warning("Unable to load project: %s", res.filepath.c_str());
					}
				}
			}

			ImGui::End();

			if (mCreatingProject)
			{
				createProjectImGui(scene, mCreatingProject);
			}
		}


		void createProjectImGui(SceneData& scene, bool& windowOpen)
		{
			ImGui::Begin("Create New Project", &windowOpen);

			ImGui::LabelText("##tmp_projectname", "Project Name:");
			ImGui::InputText("##tmp_filename", mTmpFilename, 256);
			ImGui::LabelText("##tmp_projectdir", "Project Directory:");
			ImGui::LabelText("##tmp_showfile", "%s", mNewProjectPath.path);
			ImGui::SameLine();

			if (CImGui::button("Choose Directory"))
			{
				FileDialogResult res;
				if (FileDialog::getOpenFolderName(".", res))
				{
					mNewProjectPath = PathBuilder(res.filepath.c_str()).createTmpPath();
				}
			}

			if (CImGui::button("Cancel"))
			{
				windowOpen = false;
			}
			ImGui::SameLine();
			if (CImGui::button("Create"))
			{
				if (EditorLayer::createProject(scene, mNewProjectPath, mTmpFilename))
				{
					CocoaEditor* e = static_cast<CocoaEditor*>(Application::get());
					e->setProjectLoaded();
					windowOpen = false;
				}
			}
			ImGui::End();
		}
	}
}