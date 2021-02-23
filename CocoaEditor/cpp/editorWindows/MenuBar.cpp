#include "editorWindows/MenuBar.h"
#include "core/ImGuiLayer.h"
#include "core/CocoaEditorApplication.h"
#include "editorWindows/ProjectWizard.h"
#include "editorWindows/SceneHeirarchyWindow.h"
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
			ImGui::Begin("Settings", &Settings::Editor::ShowSettingsWindow);
			CImGui::UndoableDragFloat2("Grid Size: ", Settings::Editor::GridSize);
			CImGui::UndoableColorEdit3("Grid Color: ", Settings::Editor::GridColor);
			CImGui::UndoableDragFloat("Grid Stroke Width: ", Settings::Editor::GridStrokeWidth);
			ImGui::Checkbox("Draw Grid: ", &Settings::Editor::DrawGrid);
			ImGui::End();
		}

		static bool CPathVectorGetter(void* data, int n, const char** out_text)
		{
			const std::vector<CPath>* v = (std::vector<CPath>*)data;
			*out_text = NCPath::Filename(v->at(n));
			return true;
		}

		static void StylesWindow()
		{
			ImGui::SetNextWindowSize(m_DefaultPopupSize, ImGuiCond_Once);
			ImGui::Begin("Styles", &Settings::Editor::ShowStyleSelect);
			std::vector<CPath> styles = File::GetFilesInDir(Settings::General::s_StylesDirectory);
			if (ImGui::ListBox("Styles", &Settings::Editor::SelectedStyle, CPathVectorGetter, (void*)&styles, (int)styles.size()))
			{
				ImGuiLayer::LoadStyle(styles[Settings::Editor::SelectedStyle]);
			}

			if (ImGui::CollapsingHeader("Edit Style Colors"))
			{
				CImGui::BeginCollapsingHeaderGroup();
				CImGui::UndoableColorEdit4("Accent", Settings::EditorStyle::s_Accent);
				CImGui::UndoableColorEdit4("AccentDark0", Settings::EditorStyle::s_AccentDark0);
				CImGui::UndoableColorEdit4("AccentDark1", Settings::EditorStyle::s_AccentDark1);
				CImGui::UndoableColorEdit4("Button", Settings::EditorStyle::s_Button);
				CImGui::UndoableColorEdit4("ButtonHovered", Settings::EditorStyle::s_ButtonHovered);
				CImGui::UndoableColorEdit4("Header", Settings::EditorStyle::s_Header);
				CImGui::UndoableColorEdit4("HeaderHovered", Settings::EditorStyle::s_HeaderHovered);
				CImGui::UndoableColorEdit4("HeaderActive", Settings::EditorStyle::s_HeaderActive);
				CImGui::UndoableColorEdit4("DefaultFontColor", Settings::EditorStyle::s_Font);
				CImGui::UndoableColorEdit4("DefaultFontDisabledColor", Settings::EditorStyle::s_FontDisabled);
				CImGui::UndoableColorEdit4("HighlightColor", Settings::EditorStyle::s_HighlightColor);
				CImGui::UndoableColorEdit4("MainBg", Settings::EditorStyle::s_MainBg);
				CImGui::UndoableColorEdit4("MainBgDark0", Settings::EditorStyle::s_MainBgDark0);
				CImGui::UndoableColorEdit4("MainBgDark1", Settings::EditorStyle::s_MainBgDark1);
				CImGui::UndoableColorEdit4("MainBgDark2", Settings::EditorStyle::s_MainBgDark2);
				CImGui::UndoableColorEdit4("MainBgLight0", Settings::EditorStyle::s_MainBgLight0);
				CImGui::EndCollapsingHeaderGroup();
				ImGuiLayer::ApplyStyle();

				if (CImGui::Button("Export..."))
				{
					FileDialogResult result;
					if (FileDialog::GetSaveFileName(Settings::General::s_StylesDirectory.Path, result))
					{
						ImGuiLayer::ExportCurrentStyle(NCPath::CreatePath(result.filepath));
					}
				}
			}
			ImGui::End();
		}

		void ImGui(SceneData& scene)
		{
			if (Settings::Editor::ShowSettingsWindow)
			{
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20, 20));
				SettingsWindow();
				ImGui::PopStyleVar();
			}

			if (Settings::Editor::ShowStyleSelect)
			{
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20, 20));
				StylesWindow();
				ImGui::PopStyleVar();
			}

			if (m_CreatingProject)
			{
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20, 20));
				ProjectWizard::CreateProjectImGui(scene, m_CreatingProject);
				ImGui::PopStyleVar();
			}

			ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_MenuBarButtonBg));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_MenuBarButtonBgHover));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::GetStyleColorVec4(ImGuiCol_MenuBarButtonBgActive));
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (CImGui::MenuButton("New Project"))
					{
						m_CreatingProject = true;
					}

					if (CImGui::MenuButton("Open Project"))
					{
						FileDialogResult result{};
						if (FileDialog::GetOpenFileName(".", result, { {"Jade Scenes *.jade", "*.jprj"}, {"All Files", "*.*"} }))
						{
							EditorLayer::LoadProject(scene, NCPath::CreatePath(result.filepath));
						}
					}

					if (CImGui::MenuButton("Save Scene As"))
					{
						FileDialogResult result{};
						if (FileDialog::GetSaveFileName(".", result, { {"Jade Scenes *.jade", "*.jade"}, {"All Files", "*.*"} }, ".jade"))
						{
							Settings::General::s_CurrentScene = NCPath::CreatePath(result.filepath);
							Scene::Save(scene, Settings::General::s_CurrentScene);
						}
					}

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Jade"))
				{
					if (CImGui::MenuButton("Settings"))
					{
						Settings::Editor::ShowSettingsWindow = true;
					}

					if (CImGui::MenuButton("Styles"))
					{
						Settings::Editor::ShowStyleSelect = true;
					}

					if (CImGui::MenuButton("Show Demo Window"))
					{
						Settings::Editor::ShowDemoWindow = true;
					}

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Game Objects"))
				{
					if (CImGui::MenuButton("Add Sprite Object"))
					{
						Entity entity = Scene::CreateEntity(scene);
						NEntity::AddComponent<SpriteRenderer>(entity);
						SceneHeirarchyWindow::AddNewEntity(entity);
					}

					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}
			ImGui::PopStyleColor(3);
		}
	}
}