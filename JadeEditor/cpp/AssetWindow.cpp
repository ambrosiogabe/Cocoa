#include "AssetWindow.h"
#include "jade/core/ImGuiExtended.h"
#include "jade/file/IFile.h"
#include "jade/file/JPath.h"
#include "jade/util/Settings.h"
#include "FontAwesome.h"

#include <imgui/imgui.h>

namespace Jade
{
	void AssetWindow::ImGui()
	{
		static JPath workingDirectory = IFile::GetAbsolutePath(Settings::General::s_WorkingDirectory);
		ImGui::Begin("Assets");

		ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0, 40));
		if (ImGui::Button(ICON_FA_ARROW_CIRCLE_UP))
		{
			workingDirectory = workingDirectory.GetDirectory(-1);
		}
		ImGui::SameLine();
		
		std::vector<JPath> directories = IFile::GetFoldersInDir(workingDirectory);
		for (auto dir : directories)
		{
			if (ImGui::Button((std::string(ICON_FA_FOLDER "\n") + dir.Filename()).c_str()))
			{
				workingDirectory += dir.Filename();
				break;
			}
			ImGui::SameLine();
		}

		std::vector<JPath> files = IFile::GetFilesInDir(workingDirectory);
		for (auto file : files)
		{
			if (ImGui::Button((std::string(ICON_FA_FILE "\n") + file.Filename()).c_str()))
			{
				Log::Info("CLICKED");
			}
			ImGui::SameLine();
		}

		ImGui::End();
	}
}