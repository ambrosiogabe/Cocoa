#include "externalLibs.h"
#include "jade/core/Core.h"

#include "ImGuiLayer.h"
#include "MenuBar.h"
#include "ProjectWizard.h"
#include "JadeEditorApplication.h"
#include "Gui/ImGuiExtended.h"

#include "jade/core/Entity.h"
#include "jade/file/IFileDialog.h"
#include "jade/components/components.h"
#include "jade/components/Transform.h"
#include "jade/util/Settings.h"
#include "jade/core/Application.h"
#include "jade/file/IFile.h"

namespace Jade
{
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
		std::vector<JPath> styles = IFile::GetFilesInDir(Settings::General::s_StylesDirectory);
		if (ImGui::ListBox("Styles", &m_SelectedStyle, JPathVectorGetter, (void*)&styles, (int)styles.size()))
		{
			ImGuiLayer::LoadStyle(styles[m_SelectedStyle]);
		}
		ImGui::End();
	}

	bool MenuBar::JPathVectorGetter(void* data, int n, const char** out_text)
	{
		const std::vector<JPath>* v = (std::vector<JPath>*)data;
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
				if (JImGui::MenuButton("New Project"))
				{
					m_CreatingProject = true;
				}

				if (JImGui::MenuButton("Open Project"))
				{
					FileDialogResult result{};
					if (IFileDialog::GetOpenFileName(".", result, { {"Jade Scenes *.jade", "*.jprj"}, {"All Files", "*.*"} }))
					{
						EditorLayer::LoadProject(JPath(result.filepath));
					}
				}

				if (JImGui::MenuButton("Save Scene As"))
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
				if (JImGui::MenuButton("Settings"))
				{
					m_SettingsOpen = true;
				}

				if (JImGui::MenuButton("Styles"))
				{
					// Maybe move this inside settings??
					m_StyleSelectOpen = true;
				}

				if (JImGui::MenuButton("Show Demo Window"))
				{
					m_Scene->ShowDemoWindow();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Game Objects"))
			{
				if (JImGui::MenuButton("Add Sprite Object"))
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