#pragma once
#include "externalLibs.h"
#include "Gui/ImGuiHeader.h"
#include "jade/util/Settings.h"
#include "jade/commands/ICommand.h"
#include "jade/renderer/Texture.h"

#include <imgui.h>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace JImGui
{
	bool MenuButton(const char* label, const glm::vec2& size = { 0, 0 });
	bool Button(const char* label, const glm::vec2& size = { 0, 0 });
	bool ButtonDropdown(const char* label, const char* const items[], int items_size, int& item_pressed);
	bool ImageButton(const Jade::Texture& texture, const glm::vec2& size, int framePadding = -1,
		const glm::vec4& bgColor = { 0, 0, 0, 0 }, const glm::vec4& tintColor = { 1, 1, 1, 1 });
	bool InputText(const char* label, char* buf, size_t buf_size, ImGuiInputTextFlags flags = 0,
		ImGuiInputTextCallback callback = (ImGuiInputTextCallback)0, void* user_data = (void*)0);
	bool Checkbox(const char* label, bool* checked);

	void UndoableColorEdit4(const char* label, glm::vec4& color);
	void UndoableColorEdit3(const char* label, glm::vec3& color);
	void UndoableDragFloat4(const char* label, glm::vec4& vector);
	void UndoableDragFloat3(const char* label, glm::vec3& vector);
	void UndoableDragFloat2(const char* label, glm::vec2& vector);
	void UndoableDragFloat(const char* label, float& val);
	void UndoableDragInt(const char* label, int& val);

	template <typename T>
	bool UndoableCombo(T& enumVal, const char* label, const char* const items[], int items_count)
	{
		int current_item = static_cast<int>(enumVal);
		ImGui::Text(label);
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::GetStyleColorVec4(ImGuiCol_Button));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered));
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
		ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextInverted));
		if (!ImGui::BeginCombo((std::string("##") + std::string(label)).c_str(), items[current_item], ImGuiComboFlags_None))
		{
			ImGui::PopStyleColor(4);
			return false;
		}
		ImGui::PopStyleColor(4);

		// Display items
		bool value_changed = false;
		for (int i = 0; i < items_count; i++)
		{
			ImGui::PushID((void*)(intptr_t)i);
			const bool item_selected = (i == current_item);
			const char* item_text = items[i];
			if (!item_text)
				item_text = "*Unknown item*";
			if (ImGui::Selectable(item_text, item_selected))
			{
				value_changed = true;
				current_item = i;
			}
			if (item_selected)
				ImGui::SetItemDefaultFocus();
			ImGui::PopID();
		}

		ImGui::EndCombo();

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