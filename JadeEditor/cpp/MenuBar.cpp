#include "externalLibs.h"
#include "jade/core/Core.h"

#include "MenuBar.h"
#include "ProjectWizard.h"
#include "JadeEditorApplication.h"
#include "jade/file/IFileDialog.h"
#include "jade/components/components.h"
#include "jade/components/Transform.h"
#include "jade/util/Settings.h"
#include "jade/core/Application.h"
#include "jade/core/ImGuiExtended.h"

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


	void MenuBar::ImGui()
	{
		if (m_SettingsOpen)
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20, 20));
			SettingsWindow();
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
				if (ImGui::MenuButton("New Project"))
				{
					m_CreatingProject = true;
				}

				if (ImGui::MenuButton("Open Project"))
				{
					FileDialogResult result{};
					if (IFileDialog::GetOpenFileName(".", result, { {"Jade Scenes *.jade", "*.jprj"}, {"All Files", "*.*"} }))
					{
						EditorLayer::LoadProject(JPath(result.filepath));
						Settings::General::s_CurrentScene = result.filepath;
					}
				}

				if (ImGui::MenuButton("Save Scene As"))
				{
					FileDialogResult result{};
					if (IFileDialog::GetSaveFileName(".", result, { {"Jade Scenes *.jade", "*.jade"}, {"All Files", "*.*"} }, ".jade"))
					{
						Settings::General::s_CurrentScene = result.filepath;
						Application::Get()->GetScene()->Save(result.filepath);
					}
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Jade"))
			{
				if (ImGui::MenuButton("Settings"))
				{
					m_SettingsOpen = true;
				}

				if (ImGui::MenuButton("Show Demo Window"))
				{
					Application::Get()->GetScene()->ShowDemoWindow();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Game Objects"))
			{
				if (ImGui::MenuButton("Add Sprite Object"))
				{
					entt::registry& registry = Application::Get()->GetScene()->GetRegistry();
					entt::entity entity = registry.create();
					registry.emplace<Transform>(entity, glm::vec3(), glm::vec3(1, 1, 1), glm::vec3());
					registry.emplace<SpriteRenderer>(entity, glm::vec4(1, 1, 1, 1));
					registry.emplace<AABB>(entity, glm::vec2(32, 32), glm::vec2(16, 16), glm::vec2());
				}

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		ImGui::PopStyleColor(3);
	}
}