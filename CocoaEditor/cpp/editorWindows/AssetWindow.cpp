#include "editorWindows/AssetWindow.h"
#include "core/CocoaEditorApplication.h"
#include "gui/FontAwesome.h"
#include "gui/ImGuiExtended.h"
#include "util/Settings.h"

#include "cocoa/core/AssetManager.h"
#include "cocoa/file/File.h"
#include "cocoa/file/FileDialog.h"
#include "cocoa/file/Path.h"
#include "cocoa/util/Settings.h"
#include "cocoa/core/Application.h"
#include "cocoa/scenes/Scene.h"
#include "cocoa/renderer/fonts/FontUtil.h"

namespace Cocoa
{
	namespace AssetWindow
	{
		// Internal Variables
		static glm::vec2 m_ButtonSize{ 128, 128 };
		static AssetView m_CurrentView = AssetView::TextureBrowser;

		// Forward declarations
		static void ShowMenuBar();
		static void ShowTextureBrowser();
		static void showSceneBrowser(SceneData& scene);
		static void showScriptBrowser();
		static void ShowFontBrowser();
		static bool IconButton(const char* icon, const char* label, const glm::vec2& size);
		static bool ImageButton(const Texture& texture, const char* label, const glm::vec2& size);

		void imgui(SceneData& scene)
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
				showSceneBrowser(scene);
				break;
			case AssetView::ScriptBrowser:
				showScriptBrowser();
				break;
			case AssetView::FontBrowser:
				ShowFontBrowser();
				break;
			default:
				Logger::Warning("Unkown asset view: %d", (int)m_CurrentView);
				break;
			}

			ImGui::PopStyleColor(4);
			ImGui::End();
		}

		static void ShowMenuBar()
		{
			ImGui::BeginGroup();

			std::array<const char*, (int)AssetView::Length> assetViews = { "Texture Browser", "Scene Browser", "Script Browser", "Font Browser" };
			CImGui::undoableCombo<AssetView>(m_CurrentView, "Asset View", assetViews.data(), (int)AssetView::Length);
			ImGui::EndGroup();
			ImGui::Separator();
		}

		static bool IconButton(const char* icon, const char* label, const glm::vec2& size)
		{
			ImGui::BeginGroup();
			ImGui::PushFont(Settings::EditorStyle::largeIconFont);
			bool res = ImGui::Button(icon, ImVec2(size.x, size.y));
			ImGui::PopFont();

			ImVec2 textSize = ImGui::CalcTextSize(label);
			ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(size.x / 2.0f, 0.0f) - ImVec2(textSize.x / 2.0f, 0.0f));
			ImGui::Text(label);
			ImGui::EndGroup();
			return res;
		}

		static bool ImageButton(const Texture& texture, const char* label, const glm::vec2& size)
		{
			ImGui::BeginGroup();
			ImGui::PushFont(Settings::EditorStyle::largeIconFont);
			float texRatio = (float)texture.width / (float)texture.height;
			glm::vec2 adjustedSize = { size.x * texRatio, size.y };
			bool res = CImGui::imageButton(texture, adjustedSize);
			ImGui::PopFont();

			ImVec2 textSize = ImGui::CalcTextSize(label);
			ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(size.x / 2.0f, 0.0f) - ImVec2(textSize.x / 2.0f, 0.0f));
			ImGui::Text(label);
			ImGui::EndGroup();
			return res;
		}

		static void ShowTextureBrowser()
		{
			const List<Texture>& textures = AssetManager::getAllTextures();
			int i = -1;
			for (auto& tex : textures)
			{
				i++;
				if (tex.isDefault || TextureUtil::isNull(tex))
				{
					continue;
				}

				int texResourceId = i;
				ImGui::PushID(texResourceId);
				if (ImageButton(tex, tex.path.filename, m_ButtonSize))
				{
					//m_Scene->SetActiveAsset(std::static_pointer_cast<Asset>(tex));
				}

				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
				{
					// Set payload to carry the index of our item (could be anything)
					ImGui::SetDragDropPayload("TEXTURE_HANDLE_ID", &texResourceId, sizeof(int));
					ImageButton(tex, tex.path.filename, m_ButtonSize);
					ImGui::EndDragDropSource();
				}
				ImGui::SameLine();

				ImGui::PopID();
			}

			if (IconButton(ICON_FA_PLUS, "Add Texture", m_ButtonSize))
			{
				std::string initialPath;
				FileDialogResult result;
				if (FileDialog::getOpenFileName(initialPath, result))
				{
					Texture texSpec;
					texSpec.isDefault = false;
					texSpec.magFilter = FilterMode::Nearest;
					texSpec.minFilter = FilterMode::Nearest;
					texSpec.wrapS = WrapMode::Repeat;
					texSpec.wrapT = WrapMode::Repeat;
					AssetManager::loadTextureFromFile(texSpec, PathBuilder(result.filepath.c_str()).createPath());
				}
			}
		}

		static void ShowFontBrowser()
		{
			const List<Font>& fonts = AssetManager::getAllFonts();
			int i = -1;
			for (auto& font : fonts)
			{
				i++;
				if (font.isDefault)
				{
					continue;
				}

				int fontResourceId = i;
				ImGui::PushID(fontResourceId);
				const Texture& fontTexture = AssetManager::getTexture(font.fontTexture.assetId);

				if (ImageButton(fontTexture, font.path.filename, m_ButtonSize))
				{
					//m_Scene->SetActiveAsset(std::static_pointer_cast<Asset>(tex));
				}

				if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
				{
					ImGui::SetDragDropPayload("FONT_HANDLE_ID", &fontResourceId, sizeof(int));        // Set payload to carry the index of our item (could be anything)
					ImageButton(fontTexture, font.path.filename, m_ButtonSize);
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
				//const Path& fontFile, int fontSize, const Path& outputFile, int glyphRangeStart = 0, int glyphRangeEnd = 'z' + 1, int padding = 5, int upscaleResolution = 4096
				static std::string fontPath;

				CImGui::readonlyText("Font File: ", fontPath);
				ImGui::SameLine();
				if (CImGui::button("Select Font File", { 0, 0 }, false))
				{
					FileDialogResult result;
					if (FileDialog::getOpenFileName(fontPath, result))
					{
						fontPath = result.filepath;
					}
					else
					{
						Logger::Warning("Unable to get file name for font.");
					}
				}

				static int fontSize = 32;
				CImGui::undoableDragInt("Font Size: ", fontSize);

				const Path assetsDir = PathBuilder(Settings::General::workingDirectory)
					.join("assets")
					.createPath();

				std::string outputTextureFilename = PathBuilder(fontPath.c_str()).createTmpPath().getFilenameWithoutExt() + ".png";
				Path outputTexture = PathBuilder(assetsDir)
					.join(outputTextureFilename.c_str())
					.createPath();

				// Advanced stuff...
				static int glyphRangeStart = 0;
				CImGui::undoableDragInt("Glyph Range Start: ", glyphRangeStart);

				static int glyphRangeEnd = 'z' + 1;
				CImGui::undoableDragInt("Glyph Range End: ", glyphRangeEnd);

				static int padding = 5;
				CImGui::undoableDragInt("Padding: ", padding);

				static int upscaleResolution = 4096;
				CImGui::undoableDragInt("Upscale Resolution: ", upscaleResolution);

				ImGui::NewLine();
				if (CImGui::button("Generate Font", { 0, 0 }, false))
				{
					AssetManager::loadFontFromTtfFile(PathBuilder(fontPath.c_str()).createTmpPath(), fontSize, outputTexture, glyphRangeStart, glyphRangeEnd, padding, upscaleResolution);
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();

				String::FreeString(outputTexture.path);
				String::FreeString(assetsDir.path);
			}
		}

		static void showSceneBrowser(SceneData& scene)
		{
			// TODO: Cache this!
			const Path scenesPath = PathBuilder(Settings::General::workingDirectory).join("scenes").createPath();
			auto sceneFiles = File::getFilesInDir(scenesPath);
			int sceneCount = 0;
			for (auto scenePath : sceneFiles)
			{
				ImGui::PushID(sceneCount++);
				if (IconButton(ICON_FA_FILE, scenePath.filename, m_ButtonSize))
				{
					Scene::save(scene, Settings::General::currentScene.createTmpPath());
					Scene::freeResources(scene);
					Scene::load(scene, scenePath);
				}
				ImGui::SameLine();
				ImGui::PopID();
			}

			if (IconButton(ICON_FA_PLUS, "New Scene", m_ButtonSize))
			{
				Scene::save(scene, Settings::General::currentScene.createTmpPath());
				Scene::freeResources(scene);
				static char newSceneTitle[32] = "New Scene (";
				snprintf(&newSceneTitle[11], 32 - 11, "%d).cocoa", sceneCount);

				StringBuilder sceneName;
				sceneName.Append("scenes");
				sceneName.Append(newSceneTitle);
				Settings::General::currentScene =
					PathBuilder(Settings::General::workingDirectory)
					.join(sceneName.c_str());
				Scene::save(scene, Settings::General::currentScene.createTmpPath());
				EditorLayer::saveProject();
			}

			String::FreeString(scenesPath.path);
		}

		static void showScriptBrowser()
		{
			// TODO: Cache this!!
			const Path scriptsPath = 
				PathBuilder(Settings::General::workingDirectory)
				.join("scripts")
				.createPath();
			std::vector<Path> scriptFiles = File::getFilesInDir(scriptsPath);
			int scriptCount = 0;
			for (auto script : scriptFiles)
			{
				ImGui::PushID(scriptCount++);
				if (IconButton(ICON_FA_FILE, script.filename, m_ButtonSize))
				{
					Logger::Warning("TODO: Create a way to load a script in visual studio.");
				}
				ImGui::SameLine();
				ImGui::PopID();
			}

			if (IconButton(ICON_FA_PLUS, "DefaultScript", m_ButtonSize))
			{
				char newScriptName[32] = "DefaultScript";
				snprintf(&newScriptName[13], 32 - 13, "%d", scriptCount);
				std::string scriptName = newScriptName;

				const Path cocoaDir = 
					PathBuilder(File::getSpecialAppFolder())
					.join("CocoaEngine")
					.createPath();
				const Path defaultScriptCpp = 
					PathBuilder(cocoaDir)
					.join("DefaultScript.cpp")
					.createPath();
				const Path defaultScriptH = 
					PathBuilder(cocoaDir)
					.join("DefaultScript.h")
					.createPath();

				File::copyFile(defaultScriptCpp, scriptsPath, newScriptName);
				File::copyFile(defaultScriptH, scriptsPath, newScriptName);

				String::FreeString(cocoaDir.path);
				String::FreeString(defaultScriptCpp.path);
				String::FreeString(defaultScriptH.path);
			}

			String::FreeString(scriptsPath.path);
		}
	}
}