#ifndef COCOA_EDITOR_IMGUI_EXTENDED_H
#define COCOA_EDITOR_IMGUI_EXTENDED_H
#include "cocoa/core/Core.h"
#include "externalLibs.h"

#ifdef _COCOA_SCRIPT_DLL
#include "ImGuiHeader.h"
#else
#include "gui/ImGuiHeader.h"
#endif

#include "cocoa/commands/ICommand.h"
#include "cocoa/renderer/Texture.h"

#include <imgui.h>

namespace CImGui
{
	bool menuButton(const char* label, const glm::vec2& size = { 0, 0 });
	bool button(const char* label, const glm::vec2& size = { 0, 0 }, bool invertTextColor=true);
	bool buttonDropdown(const char* label, const char* const items[], int items_size, int& item_pressed);
	bool imageButton(const Cocoa::Texture& texture, const glm::vec2& size, int framePadding = -1,
		const glm::vec4& bgColor = { 0, 0, 0, 0 }, const glm::vec4& tintColor = { 1, 1, 1, 1 });
	bool inputText(const char* label, char* buf, size_t buf_size, ImGuiInputTextFlags flags = 0,
		ImGuiInputTextCallback callback = (ImGuiInputTextCallback)0, void* user_data = (void*)0);
	bool checkbox(const char* label, bool* checked);

	bool undoableColorEdit4(const char* label, glm::vec4& color);
	bool undoableColorEdit3(const char* label, glm::vec3& color);
	bool undoableDragFloat4(const char* label, glm::vec4& vector);
	bool undoableDragFloat3(const char* label, glm::vec3& vector);
	bool undoableDragFloat2(const char* label, glm::vec2& vector);
	bool undoableDragFloat(const char* label, float& val);
	bool undoableDragInt2(const char* label, glm::ivec2& val);
	bool undoableDragInt(const char* label, int& val);

	void readonlyText(const char* label, const std::string& readonlyTextValue);

	template <typename T>
	bool undoableCombo(T& enumVal, const char* label, const char* const items[], int items_count)
	{
		int currentItem = static_cast<int>(enumVal);
		ImGui::Text(label);
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::GetStyleColorVec4(ImGuiCol_Button));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered));
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
		ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextInverted));
		if (!ImGui::BeginCombo((std::string("##") + std::string(label)).c_str(), items[currentItem], ImGuiComboFlags_None))
		{
			ImGui::PopStyleColor(4);
			return false;
		}
		ImGui::PopStyleColor(4);

		// Display items
		bool valueChanged = false;
		for (int i = 0; i < items_count; i++)
		{
			ImGui::PushID((void*)(intptr_t)i);
			const bool item_selected = (i == currentItem);
			const char* item_text = items[i];
			if (!item_text)
				item_text = "*Unknown item*";
			if (ImGui::Selectable(item_text, item_selected))
			{
				valueChanged = true;
				currentItem = i;
			}
			if (item_selected)
				ImGui::SetItemDefaultFocus();
			ImGui::PopID();
		}

		ImGui::EndCombo();

		if (valueChanged)
		{
			Cocoa::CommandHistory::addCommand(new Cocoa::ChangeEnumCommand<T>(enumVal, static_cast<T>(currentItem)));
			Cocoa::CommandHistory::setNoMergeMostRecent();
		}
		return valueChanged;
	}

	void beginCollapsingHeaderGroup();
	void endCollapsingHeaderGroup();
	ImVec4 from(const glm::vec4& vec4);
	ImVec2 from(const glm::vec2& vec2);

	bool beginDragDropTargetCurrentWindow();
}

#endif
