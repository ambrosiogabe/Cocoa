#include "AssetWindow.h"
#include "jade/core/ImGuiExtended.h"
#include "jade/core/AssetManager.h"
#include "jade/file/IFile.h"
#include "jade/file/IFileDialog.h"
#include "jade/file/JPath.h"
#include "jade/util/Settings.h"
#include "FontAwesome.h"

#include <imgui/imgui.h>

namespace Jade
{
	void AssetWindow::ImGui()
	{
		ImGui::Begin("Assets");
		ShowMenuBar();

		ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_FrameBgActive));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_FrameBgHovered));
		ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_Text));
		ImGui::PushStyleColor(ImGuiCol_TextDisabled, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));

		switch (m_CurrentView) 
		{
		case AssetView::FileBrowser:
			ShowFileBrowser();
			break;
		case AssetView::TextureBrowser:
			ShowTextureBrowser();
			break;
		}

		ImGui::PopStyleColor(4);
		ImGui::End();
	}

	void AssetWindow::ShowMenuBar()
	{
		ImGui::BeginGroup();

		std::array<const char*, (int)AssetView::Length> assetViews = { "FileBrowser", "TextureBrowser" };
		ImGui::UndoableCombo<AssetView>(m_CurrentView, "Asset View", assetViews.data(), (int)AssetView::Length);
		ImGui::EndGroup();
		ImGui::Separator();
	}

	bool AssetWindow::IconButton(const char* icon, const char* label, const glm::vec2& size)
	{
		ImGui::BeginGroup();
		ImGui::PushFont(Settings::EditorStyle::s_LargeIconFont);
		bool res = ImGui::Button(icon, ImVec2(size.x, size.y));
		ImGui::PopFont();
		
		ImVec2 textSize = ImGui::CalcTextSize(label);
		ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(size.x / 2.0f, 0.0f) -  ImVec2(textSize.x / 2.0f, 0.0f));
		ImGui::Text(label);
		ImGui::EndGroup();
		return res;
	}

	bool AssetWindow::ImageButton(Texture* texture, const char* label, const glm::vec2& size)
	{
		ImGui::BeginGroup();
		ImGui::PushFont(Settings::EditorStyle::s_LargeIconFont);
		bool res = ImGui::JImageButton(*texture, size);
		ImGui::PopFont();

		ImVec2 textSize = ImGui::CalcTextSize(label);
		ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(size.x / 2.0f, 0.0f) - ImVec2(textSize.x / 2.0f, 0.0f));
		ImGui::Text(label);
		ImGui::EndGroup();
		return res;
	}

	void AssetWindow::ShowFileBrowser()
	{
		static JPath workingDirectory = IFile::GetAbsolutePath(Settings::General::s_WorkingDirectory);

		ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(0, 40));
		if (IconButton(ICON_FA_ARROW_CIRCLE_UP, "", m_ButtonSize))
		{
			workingDirectory = workingDirectory.GetDirectory(-1);
		}
		ImGui::SameLine();

		std::vector<JPath> directories = IFile::GetFoldersInDir(workingDirectory);
		for (auto dir : directories)
		{
			if (IconButton(ICON_FA_FOLDER, dir.Filename(), m_ButtonSize))
			{
				workingDirectory += dir.Filename();
				break;
			}
			ImGui::SameLine();
		}

		std::vector<JPath> files = IFile::GetFilesInDir(workingDirectory);
		for (auto file : files)
		{
			if (IconButton(ICON_FA_FILE, file.Filename(), m_ButtonSize))
			{
				Log::Info("CLICKED");
			}
			ImGui::SameLine();
		}
	}

	void AssetWindow::ShowTextureBrowser()
	{
		std::vector<std::shared_ptr<Texture>> textures = AssetManager::GetAllAssets<Texture>(AssetManager::GetScene());
		for (auto tex : textures)
		{
			int texResourceId = tex->GetResourceId();
			ImGui::PushID(texResourceId);
			ImageButton(tex.get(), tex->GetFilepath().Filename(), m_ButtonSize);
			// Our buttons are both drag sources and drag targets here!
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
			{
				ImGui::SetDragDropPayload("TEXTURE_HANDLE_ID", &texResourceId, sizeof(int));        // Set payload to carry the index of our item (could be anything)
				ImageButton(tex.get(), tex->GetFilepath().Filename(), m_ButtonSize);
				ImGui::EndDragDropSource();
			}
			ImGui::SameLine();

			ImGui::PopID();
		}

		if (IconButton(ICON_FA_PLUS, "Add Texture", m_ButtonSize))
		{
			std::string initialPath;
			FileDialogResult result;
			if (IFileDialog::GetOpenFileName(initialPath, result))
			{
				AssetManager::LoadTextureFromFile(JPath(result.filepath));
			}
		}
	}
}