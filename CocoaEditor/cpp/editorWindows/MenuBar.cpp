#include "editorWindows/MenuBar.h"
#include "core/ImGuiLayer.h"
#include "core/CocoaEditorApplication.h"
#include "editorWindows/ProjectWizard.h"
#include "gui/ImGuiExtended.h"

#include "cocoa/core/Entity.h"
#include "cocoa/file/IFileDialog.h"
#include "cocoa/components/components.h"
#include "cocoa/components/Transform.h"
#include "cocoa/util/Settings.h"
#include "cocoa/core/Application.h"
#include "cocoa/file/IFile.h"

namespace Cocoa
{
	bool MenuBar::ShowDemoWindow = false;

	void MenuBar::SettingsWindow()
	{
		ImGui::Begin("Settings", &m_SettingsOpen);
		if (ImGui::DragInt2("Grid Size: ", m_GridSize))
		{
			Settings::General::s_GridSizeX = m_GridSize[0];
			Settings::General::s_GridSizeY = m_GridSize[1];
		}

		ImGui::Checkbox("Draw Grid: ", &Settings::General::s_DrawGrid);
		ImGui::End();
	}

	void MenuBar::StylesWindow()
	{
		ImGui::Begin("Styles");
		std::vector<CPath> styles = IFile::GetFilesInDir(Settings::General::s_StylesDirectory);
		if (ImGui::ListBox("Styles", &m_SelectedStyle, CPathVectorGetter, (void*)&styles, (int)styles.size()))
		{
			ImGuiLayer::LoadStyle(styles[m_SelectedStyle]);
		}
		ImGui::End();
	}

	bool MenuBar::CPathVectorGetter(void* data, int n, const char** out_text)
	{
		const std::vector<CPath>* v = (std::vector<CPath>*)data;
		*out_text = v->at(n).Filename();
		return true;
	}


	void MenuBar::ImGui()
	{
		if (m_SettingsOpen)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20, 20));
			SettingsWindow();
			ImGui::PopStyleVar();
		}

		if (m_StyleSelectOpen)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20, 20));
			StylesWindow();
			ImGui::PopStyleVar();
		}

		if (m_CreatingProject)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20, 20));
			if (ProjectWizard::CreateProjectImGui())
			{
				m_CreatingProject = false;
			}
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
					if (IFileDialog::GetOpenFileName(".", result, { {"Jade Scenes *.jade", "*.jprj"}, {"All Files", "*.*"} }))
					{
						EditorLayer::LoadProject(CPath(result.filepath));
					}
				}

				if (CImGui::MenuButton("Save Scene As"))
				{
					FileDialogResult result{};
					if (IFileDialog::GetSaveFileName(".", result, { {"Jade Scenes *.jade", "*.jade"}, {"All Files", "*.*"} }, ".jade"))
					{
						Settings::General::s_CurrentScene = result.filepath;
						m_Scene->Save(result.filepath);
					}
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Jade"))
			{
				if (CImGui::MenuButton("Settings"))
				{
					m_SettingsOpen = true;
				}

				if (CImGui::MenuButton("Styles"))
				{
					// Maybe move this inside settings??
					m_StyleSelectOpen = true;
				}

				if (CImGui::MenuButton("Show Demo Window"))
				{
					ShowDemoWindow = true;
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Game Objects"))
			{
				if (CImGui::MenuButton("Add Sprite Object"))
				{
					Entity entity = m_Scene->CreateEntity();
					entity.AddComponent<SpriteRenderer>();
				}

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		ImGui::PopStyleColor(3);
	}
}