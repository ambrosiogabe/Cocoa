#include "AssetWindow.h"
#include "jade/core/ImGuiExtended.h"
#include "jade/core/AssetManager.h"
#include "jade/file/IFile.h"
#include "jade/file/IFileDialog.h"
#include "jade/file/JPath.h"
#include "jade/util/Settings.h"
#include "FontAwesome.h"
#include "jade/core/Application.h"

#include <imgui.h>

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
		case AssetView::TextureBrowser:
			ShowTextureBrowser();
			break;
		default:
			Log::Warning("Unkown asset view: %d", (int)m_CurrentView);
			break;
		}

		ImGui::PopStyleColor(4);
		ImGui::End();
	}

	void AssetWindow::ShowMenuBar()
	{
		ImGui::BeginGroup();

		std::array<const char*, (int)AssetView::Length> assetViews = { "TextureBrowser", "ScriptBrowser" };
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

	void AssetWindow::ShowTextureBrowser()
	{
		std::vector<std::shared_ptr<Texture>> textures = AssetManager::GetAllAssets<Texture>(AssetManager::GetScene());
		for (auto tex : textures)
		{
			if (tex->IsDefault())
			{
				continue;
			}

			int texResourceId = tex->GetResourceId();
			ImGui::PushID(texResourceId);
			if (ImageButton(tex.get(), tex->GetFilepath().Filename(), m_ButtonSize))
			{
				Application::Get()->GetScene()->SetActiveAsset(std::static_pointer_cast<Asset>(tex));
			}

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