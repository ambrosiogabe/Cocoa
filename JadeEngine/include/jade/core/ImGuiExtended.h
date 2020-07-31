#include "externalLibs.h"
#include "jade/util/Settings.h"
#include "jade/commands/ICommand.h"

#include <imgui.h>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace ImGui {
	ImVec4 From(const glm::vec4& vec4);

	bool MenuButton(const char* label, const glm::vec2& size = {0, 0});
	bool JButton(const char* label, const glm::vec2& size = {0, 0});
	bool JButtonDropdown(const char* label, const char* const items[], int items_size, int& item_pressed);
	bool JImageButton(const Jade::Sprite& texture, const glm::vec2& size, int framePadding = -1, 
		const glm::vec4& bgColor={0, 0, 0, 0}, const glm::vec4& tintColor={1, 1, 1, 1});

	void UndoableColorEdit4(const char* label, glm::vec4& color);
	void UndoableColorEdit3(const char* label, glm::vec3& color);
	void UndoableDragFloat4(const char* label, glm::vec4& vector);
	void UndoableDragFloat3(const char* label, glm::vec3& vector);
	void UndoableDragFloat2(const char* label, glm::vec2& vector);
	void UndoableDragFloat(const char* label, float& val);

	template <typename T>
	bool UndoableCombo(T enumVal, const char* label, const char* const items[], int items_count, int popup_max_items_in_height = -1)
	{
		int current_item = static_cast<int>(enumVal);
		ImGui::Text(label);
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_FrameBg, GetStyleColorVec4(ImGuiCol_Button));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, GetStyleColorVec4(ImGuiCol_ButtonHovered));
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, GetStyleColorVec4(ImGuiCol_ButtonActive));
		ImGui::PushStyleColor(ImGuiCol_Text, GetStyleColorVec4(ImGuiCol_TextInverted));
        if (!BeginCombo((std::string("##") + std::string(label)).c_str(), items[current_item], ImGuiComboFlags_None)) {
			ImGui::PopStyleColor(4);
            return false;
		}
		ImGui::PopStyleColor(4);

        // Display items
        bool value_changed = false;
        for (int i = 0; i < items_count; i++)
        {
            PushID((void*)(intptr_t)i);
            const bool item_selected = (i == current_item);
            const char* item_text = items[i];
            if (!item_text)
                item_text = "*Unknown item*";
            if (Selectable(item_text, item_selected))
            {
                value_changed = true;
                current_item = i;
            }
            if (item_selected)
                SetItemDefaultFocus();
            PopID();
        }

        EndCombo();

		if (value_changed)
		{
			Jade::CommandHistory::AddCommand(new Jade::ChangeEnumCommand<T>(enumVal, static_cast<T>(current_item)));
			Jade::CommandHistory::SetNoMergeMostRecent();
		}
        return value_changed;
	}

	void BeginCollapsingHeaderGroup();
	void EndCollapsingHeaderGroup();
}