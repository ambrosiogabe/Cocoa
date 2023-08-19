#include "editorWindows/MenuBar.h"
#include "core/ImGuiLayer.h"
#include "core/CocoaEditorApplication.h"
#include "editorWindows/ProjectWizard.h"
#include "editorWindows/SceneHierarchyWindow.h"
#include "gui/ImGuiExtended.h"
#include "util/Settings.h"

#include "cocoa/core/Entity.h"
#include "cocoa/core/Application.h"
#include "cocoa/file/FileDialog.h"
#include "cocoa/file/File.h"
#include "cocoa/components/SpriteRenderer.h"
#include "cocoa/components/Transform.h"
#include "cocoa/util/Settings.h"

namespace Cocoa
{
	namespace MenuBar
	{
		// Internal variables
		static bool m_CreatingProject = false;
		static glm::vec2 m_DefaultPopupSize = { 900, 600 };
		static bool mShowSettings = true;

		static void SettingsWindow()
		{
			ImGui::SetNextWindowSize(m_DefaultPopupSize, ImGuiCond_Once);
			ImGui::Begin("Settings", &Settings::Editor::showSettingsWindow);
			CImGui::undoableDragFloat2("Grid Size: ", Settings::Editor::gridSize);
			CImGui::undoableColorEdit3("Grid Color: ", Settings::Editor::gridColor);
			CImGui::undoableDragFloat("Grid Stroke Width: ", Settings::Editor::gridStrokeWidth);
			ImGui::Checkbox("Draw Grid: ", &Settings::Editor::drawGrid);
			ImGui::End();
		}

		static bool CPathVectorGetter(void* data, int n, const char** out_text)
		{
			const std::vector<std::filesystem::path>* v = (std::vector<std::filesystem::path>*)data;
			*out_text = v->at(n).filename().string().c_str();
			return true;
		}

		static void StylesWindow()
		{
			ImGui::SetNextWindowSize(m_DefaultPopupSize, ImGuiCond_Once);
			ImGui::Begin("Styles", &Settings::Editor::showStyleSelect);
			std::vector<std::filesystem::path> styles = File::getFilesInDir(Settings::General::stylesDirectory);
			if (ImGui::ListBox("Styles", &Settings::Editor::selectedStyle, CPathVectorGetter, (void*)&styles, (int)styles.size()))
			{
				ImGuiLayer::loadStyle(styles[Settings::Editor::selectedStyle]);
			}

			if (ImGui::CollapsingHeader("Edit Style Colors"))
			{
				CImGui::beginCollapsingHeaderGroup();
				CImGui::undoableColorEdit4("Accent", Settings::EditorStyle::accent);
				CImGui::undoableColorEdit4("AccentDark0", Settings::EditorStyle::accentDark0);
				CImGui::undoableColorEdit4("AccentDark1", Settings::EditorStyle::accentDark1);
				CImGui::undoableColorEdit4("Button", Settings::EditorStyle::button);
				CImGui::undoableColorEdit4("ButtonHovered", Settings::EditorStyle::buttonHovered);
				CImGui::undoableColorEdit4("Header", Settings::EditorStyle::header);
				CImGui::undoableColorEdit4("HeaderHovered", Settings::EditorStyle::headerHovered);
				CImGui::undoableColorEdit4("HeaderActive", Settings::EditorStyle::headerActive);
				CImGui::undoableColorEdit4("DefaultFontColor", Settings::EditorStyle::font);
				CImGui::undoableColorEdit4("DefaultFontDisabledColor", Settings::EditorStyle::fontDisabled);
				CImGui::undoableColorEdit4("HighlightColor", Settings::EditorStyle::highlightColor);
				CImGui::undoableColorEdit4("MainBg", Settings::EditorStyle::mainBg);
				CImGui::undoableColorEdit4("MainBgDark0", Settings::EditorStyle::mainBgDark0);
				CImGui::undoableColorEdit4("MainBgDark1", Settings::EditorStyle::mainBgDark1);
				CImGui::undoableColorEdit4("MainBgDark2", Settings::EditorStyle::mainBgDark2);
				CImGui::undoableColorEdit4("MainBgLight0", Settings::EditorStyle::mainBgLight0);
				CImGui::endCollapsingHeaderGroup();
				ImGuiLayer::applyStyle();

				if (CImGui::button("Export..."))
				{
					FileDialogResult result;
					if (FileDialog::getSaveFileName(Settings::General::stylesDirectory.string(), result))
					{
						ImGuiLayer::exportCurrentStyle(result.filepath);
					}
				}
			}
			ImGui::End();
		}

		void imgui(SceneData& scene)
		{
			if (Settings::Editor::showSettingsWindow)
			{
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20, 20));
				SettingsWindow();
				ImGui::PopStyleVar();
			}

			if (Settings::Editor::showStyleSelect)
			{
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20, 20));
				StylesWindow();
				ImGui::PopStyleVar();
			}

			if (m_CreatingProject)
			{
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20, 20));
				ProjectWizard::createProjectImGui(scene, m_CreatingProject);
				ImGui::PopStyleVar();
			}

			ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_MenuBarButtonBg));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_MenuBarButtonBgHover));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::GetStyleColorVec4(ImGuiCol_MenuBarButtonBgActive));
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (CImGui::menuButton("New Project"))
					{
						m_CreatingProject = true;
					}

					if (CImGui::menuButton("Open Project"))
					{
						FileDialogResult result{};
						if (FileDialog::getOpenFileName(".", result, { {"Cocoa Projects *.cocoa", "*.cprj"}, {"All Files", "*.*"} }))
						{
							EditorLayer::loadProject(scene, result.filepath);
						}
					}

					if (CImGui::menuButton("Save Scene As"))
					{
						FileDialogResult result{};
						if (FileDialog::getSaveFileName(".", result, { {"Jade Scenes *.jade", "*.jade"}, {"All Files", "*.*"} }, ".jade"))
						{
							Settings::General::currentScene = result.filepath;
							Scene::save(scene, Settings::General::currentScene);
						}
					}

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Jade"))
				{
					if (CImGui::menuButton("Settings"))
					{
						Settings::Editor::showSettingsWindow = true;
					}

					if (CImGui::menuButton("Styles"))
					{
						Settings::Editor::showStyleSelect = true;
					}

					if (CImGui::menuButton("Show Demo Window"))
					{
						Settings::Editor::showDemoWindow = true;
					}

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Game Objects"))
				{
					if (CImGui::menuButton("Add Sprite Object"))
					{
						Entity entity = Scene::createEntity(scene);
						NEntity::addComponent<SpriteRenderer>(entity);
						SceneHierarchyWindow::addNewEntity(entity);
					}

					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}
			ImGui::PopStyleColor(3);
		}
	}
}