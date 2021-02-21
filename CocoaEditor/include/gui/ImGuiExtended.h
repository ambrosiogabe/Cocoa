#pragma once
#include "cocoa/core/Core.h"
#include "externalLibs.h"

#ifdef _COCOA_SCRIPT_DLL
#include "ImGuiHeader.h"
#else
#include "gui/ImGuiHeader.h"
#endif

#include "cocoa/util/Settings.h"
#include "cocoa/commands/ICommand.h"
#include "cocoa/renderer/Texture.h"

#include <imgui.h>

namespace CImGui
{
	bool MenuButton(const char* label, const glm::vec2& size = { 0, 0 });
	bool Button(const char* label, const glm::vec2& size = { 0, 0 }, bool invertTextColor=true);
	bool ButtonDropdown(const char* label, const char* const items[], int items_size, int& item_pressed);
	bool ImageButton(const Cocoa::Texture& texture, const glm::vec2& size, int framePadding = -1,
		const glm::vec4& bgColor = { 0, 0, 0, 0 }, const glm::vec4& tintColor = { 1, 1, 1, 1 });
	bool InputText(const char* label, char* buf, size_t buf_size, ImGuiInputTextFlags flags = 0,
		ImGuiInputTextCallback callback = (ImGuiInputTextCallback)0, void* user_data = (void*)0);
	bool Checkbox(const char* label, bool* checked);

	bool UndoableColorEdit4(const char* label, glm::vec4& color);
	bool UndoableColorEdit3(const char* label, glm::vec3& color);
	bool UndoableDragFloat4(const char* label, glm::vec4& vector);
	bool UndoableDragFloat3(const char* label, glm::vec3& vector);
	bool UndoableDragFloat2(const char* label, glm::vec2& vector);
	bool UndoableDragFloat(const char* label, float& val);
	bool UndoableDragInt2(const char* label, glm::ivec2& val);
	bool UndoableDragInt(const char* label, int& val);

	void ReadonlyText(const char* label, const std::string& readonlyTextValue);

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
			Cocoa::CommandHistory::AddCommand(new Cocoa::ChangeEnumCommand<T>(enumVal, static_cast<T>(current_item)));
			Cocoa::CommandHistory::SetNoMergeMostRecent();
		}
		return value_changed;
	}

	void BeginCollapsingHeaderGroup();
	void EndCollapsingHeaderGroup();
	ImVec4 From(const glm::vec4& vec4);
	ImVec2 From(const glm::vec2& vec2);
}