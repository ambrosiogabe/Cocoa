#include "externalLibs.h"
#include "jade/core/Core.h"

#include "MenuBar.h"
#include "AssetWizard.h"
#include "jade/file/IFileDialog.h"
#include "jade/components/components.h"
#include "jade/components/Transform.h"
#include "jade/util/Settings.h"
#include "jade/core/Application.h"

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
			SettingsWindow();
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::Button("New Project"))
				{
					AssetWizard::NewProject();
				}

				if (ImGui::Button("Open Scene"))
				{
					FileDialogResult result{};
					if (IFileDialog::GetOpenFileName(".", result, { {"Jade Scenes *.jade", "*.jade"}, {"All Files", "*.*"} }))
					{
						Application::Get()->GetScene()->Load(result.filepath);
						Settings::General::s_CurrentScene = result.filepath;
					}
					//ImGui::EndMenu();
				}

				if (ImGui::Button("Save Scene As"))
				{
					FileDialogResult result{};
					if (IFileDialog::GetSaveFileName(".", result, { {"Jade Scenes *.jade", "*.jade"}, {"All Files", "*.*"} }, ".jade"))
					{
						Settings::General::s_CurrentScene = result.filepath;
						Application::Get()->GetScene()->Save(result.filepath);
					}
					//ImGui::EndMenu();
				}

				if (ImGui::Button("Import Asset"))
				{
					AssetWizard::BeginImport();
					//ImGui::EndMenu();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Jade"))
			{
				if (ImGui::Button("Settings"))
				{
					m_SettingsOpen = true;
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Game Objects"))
			{
				if (ImGui::Button("Add Sprite Object"))
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
	}
}