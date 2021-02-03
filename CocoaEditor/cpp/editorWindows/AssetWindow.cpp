#include "editorWindows/AssetWindow.h"
#include "core/CocoaEditorApplication.h"
#include "gui/FontAwesome.h"
#include "gui/ImGuiExtended.h"
#include "util/Settings.h"

#include "cocoa/core/AssetManager.h"
#include "cocoa/file/IFile.h"
#include "cocoa/file/IFileDialog.h"
#include "cocoa/file/CPath.h"
#include "cocoa/util/Settings.h"
#include "cocoa/core/Application.h"
#include "cocoa/scenes/Scene.h"
#include "cocoa/renderer/fonts/FontUtil.h"

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
		case AssetView::FontBrowser:
			ShowFontBrowser();
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

		std::array<const char*, (int)AssetView::Length> assetViews = { "Texture Browser", "Scene Browser", "Script Browser", "Font Browser" };
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

	bool AssetWindow::ImageButton(const Texture& texture, const char* label, const glm::vec2& size)
	{
		ImGui::BeginGroup();
		ImGui::PushFont(Settings::EditorStyle::s_LargeIconFont);
		bool res = CImGui::ImageButton(texture, size);
		ImGui::PopFont();

		ImVec2 textSize = ImGui::CalcTextSize(label);
		ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(size.x / 2.0f, 0.0f) - ImVec2(textSize.x / 2.0f, 0.0f));
		ImGui::Text(label);
		ImGui::EndGroup();
		return res;
	}

	void AssetWindow::ShowTextureBrowser()
	{
		const std::vector<Texture>& textures = AssetManager::GetAllTextures();
		int i = -1;
		for (auto& tex : textures)
		{
			i++;
			if (tex.IsDefault || TextureUtil::IsNull(tex))
			{
				continue;
			}

			int texResourceId = i;
			ImGui::PushID(texResourceId);
			if (ImageButton(tex, tex.Path.Filename(), m_ButtonSize))
			{
				//m_Scene->SetActiveAsset(std::static_pointer_cast<Asset>(tex));
			}

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
			{
				ImGui::SetDragDropPayload("TEXTURE_HANDLE_ID", &texResourceId, sizeof(int));        // Set payload to carry the index of our item (could be anything)
				ImageButton(tex, tex.Path.Filename(), m_ButtonSize);
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
				Texture texSpec;
				texSpec.IsDefault = false;
				texSpec.MagFilter = FilterMode::Nearest;
				texSpec.MinFilter = FilterMode::Nearest;
				texSpec.WrapS = WrapMode::Repeat;
				texSpec.WrapT = WrapMode::Repeat;
				AssetManager::LoadTextureFromFile(texSpec, CPath(result.filepath));
			}
		}
	}

	void AssetWindow::ShowFontBrowser()
	{
		const std::vector<Font>& fonts = AssetManager::GetAllFonts();
		int i = -1;
		for (auto& font : fonts)
		{
			i++;
			if (font.IsDefault())
			{
				continue;
			}

			int fontResourceId = i;
			ImGui::PushID(fontResourceId);
			const Texture& fontTexture = AssetManager::GetTexture(font.m_FontTexture.m_AssetId);

			if (ImageButton(fontTexture, font.m_Path.Filename(), m_ButtonSize))
			{
				//m_Scene->SetActiveAsset(std::static_pointer_cast<Asset>(tex));
			}

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
			{
				ImGui::SetDragDropPayload("FONT_HANDLE_ID", &fontResourceId, sizeof(int));        // Set payload to carry the index of our item (could be anything)
				ImageButton(fontTexture, font.m_Path.Filename(), m_ButtonSize);
				ImGui::EndDragDropSource();
			}
			ImGui::SameLine();

			ImGui::PopID();
		}

		if (IconButton(ICON_FA_PLUS, "Add Font", m_ButtonSize))
		{
			ImGui::OpenPopup("FontCreator");
		}

		if (ImGui::BeginPopup("FontCreator", ImGuiWindowFlags_NoDocking))
		{
			//const CPath& fontFile, int fontSize, const CPath& outputFile, int glyphRangeStart = 0, int glyphRangeEnd = 'z' + 1, int padding = 5, int upscaleResolution = 4096
			static std::string fontPath;
			
			CImGui::ReadonlyText("Font File: ", fontPath);
			ImGui::SameLine();
			if (CImGui::Button("Select Font File", {0, 0}, false))
			{
				FileDialogResult result;
				if (IFileDialog::GetOpenFileName(fontPath, result))
				{
					fontPath = result.filepath;
				}
				else
				{
					Log::Warning("Unable to get file name for font.");
				}
			}

			static int fontSize = 32;
			CImGui::UndoableDragInt("Font Size: ", fontSize);

			CPath outputTexture = Settings::General::s_WorkingDirectory + "assets" + (std::string(CPath(fontPath).GetFilenameWithoutExt()) + ".png");

			// Advanced stuff...
			static int glyphRangeStart = 0;
			CImGui::UndoableDragInt("Glyph Range Start: ", glyphRangeStart);

			static int glyphRangeEnd = 'z' + 1;
			CImGui::UndoableDragInt("Glyph Range End: ", glyphRangeEnd);

			static int padding = 5;
			CImGui::UndoableDragInt("Padding: ", padding);

			static int upscaleResolution = 4096;
			CImGui::UndoableDragInt("Upscale Resolution: ", upscaleResolution);

			ImGui::NewLine();
			if (CImGui::Button("Generate Font", {0, 0}, false))
			{
				AssetManager::LoadFontFromTtfFile(fontPath, fontSize, outputTexture, glyphRangeStart, glyphRangeEnd, padding, upscaleResolution);
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
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

		if (IconButton(ICON_FA_PLUS, "DefaultScript", m_ButtonSize))
		{
			char newScriptName[32] = "DefaultScript";
			snprintf(&newScriptName[13], 32 - 13, "%d", scriptCount);
			std::string scriptName = newScriptName;
			IFile::CopyFile(IFile::GetSpecialAppFolder() + "CocoaEngine" + "DefaultScript.cpp", CPath(Settings::General::s_CurrentProject.GetDirectory(-1)) + "scripts", newScriptName);
			IFile::CopyFile(IFile::GetSpecialAppFolder() + "CocoaEngine" + "DefaultScript.h", CPath(Settings::General::s_CurrentProject.GetDirectory(-1)) + "scripts", newScriptName);
		}
	}
}