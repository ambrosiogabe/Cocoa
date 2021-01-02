#include "EditorWindows/AssetWindow.h"
#include "Gui/ImGuiExtended.h"
#include "FontAwesome.h"
#include "Util/Settings.h"
#include "CocoaEditorApplication.h"

#include "cocoa/core/AssetManager.h"
#include "cocoa/file/IFile.h"
#include "cocoa/file/IFileDialog.h"
#include "cocoa/file/CPath.h"
#include "cocoa/util/Settings.h"
#include "cocoa/core/Application.h"
#include "cocoa/scenes/Scene.h"

namespace Cocoa
{
	AssetWindow::AssetWindow(Scene* scene) 
		: m_Scene(scene) {}

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
		case AssetView::SceneBrowser:
			ShowSceneBrowser();
			break;
		case AssetView::ScriptBrowser:
			ShowScriptBrowser();
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

		std::array<const char*, (int)AssetView::Length> assetViews = { "TextureBrowser", "SceneBrowser", "ScriptBrowser" };
		CImGui::UndoableCombo<AssetView>(m_CurrentView, "Asset View", assetViews.data(), (int)AssetView::Length);
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
		bool res = CImGui::ImageButton(*texture, size);
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
				//m_Scene->SetActiveAsset(std::static_pointer_cast<Asset>(tex));
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
				AssetManager::LoadTextureFromFile(CPath(result.filepath));
			}
		}
	}

	void AssetWindow::ShowSceneBrowser()
	{
		auto sceneFiles = IFile::GetFilesInDir(Settings::General::s_WorkingDirectory + "scenes");
		int sceneCount = 0;
		for (auto scene : sceneFiles)
		{
			ImGui::PushID(sceneCount++);
			if (IconButton(ICON_FA_FILE, scene.Filename(), m_ButtonSize))
			{
				m_Scene->Save(Settings::General::s_CurrentScene);
				m_Scene->Load(scene);
			}
			ImGui::SameLine();
			ImGui::PopID();
		}

		if (IconButton(ICON_FA_PLUS, "New Scene", m_ButtonSize))
		{
			m_Scene->Save(Settings::General::s_CurrentScene);
			m_Scene->Reset();
			char newSceneTitle[32] = "New Scene (";
			snprintf(&newSceneTitle[11], 32 - 11, "%d).cocoa", sceneCount);
			Settings::General::s_CurrentScene = Settings::General::s_WorkingDirectory + "scenes" + std::string(newSceneTitle);
			m_Scene->Save(Settings::General::s_CurrentScene);
			CocoaEditor* editor = static_cast<CocoaEditor*>(Application::Get());
			editor->GetEditorLayer()->SaveProject();
		}
	}

	void AssetWindow::ShowScriptBrowser()
	{
		auto scriptFiles = IFile::GetFilesInDir(Settings::General::s_WorkingDirectory + "scripts");
		int scriptCount = 0;
		for (auto script : scriptFiles)
		{
			ImGui::PushID(scriptCount++);
			if (IconButton(ICON_FA_FILE, script.Filename(), m_ButtonSize))
			{
				Log::Warning("TODO: Create a way to load a script in visual studio.");
			}
			ImGui::SameLine();
			ImGui::PopID();
		}

		if (IconButton(ICON_FA_PLUS, "New Script", m_ButtonSize))
		{
			char newScriptName[32] = "New Script ";
			snprintf(&newScriptName[11], 32 - 12, "%d", scriptCount);
			std::string scriptName = newScriptName;
			IFile::CopyFile(IFile::GetSpecialAppFolder() + "CocoaEngine" + "DefaultScript.cpp", CPath(Settings::General::s_CurrentProject.GetDirectory(-1)) + "scripts", newScriptName);
			IFile::CopyFile(IFile::GetSpecialAppFolder() + "CocoaEngine" + "DefaultScript.h", CPath(Settings::General::s_CurrentProject.GetDirectory(-1)) + "scripts", newScriptName);
		}
	}
}