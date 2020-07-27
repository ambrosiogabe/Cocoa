#include "AssetWindow.h"
#include "jade/core/ImGuiExtended.h"
#include "jade/file/IFile.h"
#include "jade/util/Settings.h"
#include "FontAwesome.h"

#include <imgui/imgui.h>

namespace Jade
{
	void AssetWindow::ImGui()
	{
		static std::string workingDirectory = Settings::General::s_WorkingDirectory;
		ImGui::Begin("Assets");

		ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0, 40));
		if (ImGui::Button(ICON_FA_ARROW_CIRCLE_UP))
		{
			Log::Info("UP DIR");
		}
		ImGui::SameLine();
		
		std::vector<std::string> directories = IFile::GetFoldersInDir(workingDirectory.c_str());
		for (auto dir : directories)
		{
			if (ImGui::Button((std::string(ICON_FA_FOLDER "\n") + dir).c_str()))
			{
				workingDirectory = dir;
				break;
			}
			ImGui::SameLine();
		}

		std::vector<std::string> files = IFile::GetFilesInDir(workingDirectory.c_str());
		for (auto file : files)
		{
			if (ImGui::Button((std::string(ICON_FA_FILE "\n") + file).c_str()))
			{
				Log::Info("CLICKED");
			}
			ImGui::SameLine();
		}

		ImGui::End();
	}
}