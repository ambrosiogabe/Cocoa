#ifdef _COCOA_SCRIPT_DLL
#include "ImGuiExtended.h"
#else
#include "gui/ImGuiExtended.h"
#endif

#include "cocoa/core/Core.h"

namespace CImGui
{
	static float textPadding = 0;
	bool undoableColorEdit4(const char* label, glm::vec4& color)
	{
		glm::vec4 tmp = glm::vec4(color);
		ImGui::Text(label);

		if (ImGui::GetItemRectSize().x > textPadding)
		{
			textPadding = ImGui::GetItemRectSize().x;
		}

		ImGui::SameLine(textPadding);
		bool result = ImGui::ColorEdit4((std::string("##") + std::string(label)).c_str(), glm::value_ptr(tmp));
		if (result)
		{
			Cocoa::CommandHistory::addCommand(new Cocoa::ChangeVec4Command(color, tmp));
		}
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			Cocoa::CommandHistory::setNoMergeMostRecent();
		}

		return result;
	}

	bool undoableColorEdit3(const char* label, glm::vec3& color)
	{
		glm::vec3 tmp = glm::vec3(color);
		ImGui::Text(label);

		if (ImGui::GetItemRectSize().x > textPadding)
		{
			textPadding = ImGui::GetItemRectSize().x;
		}

		ImGui::SameLine(textPadding);
		bool result = ImGui::ColorEdit3((std::string("##") + std::string(label)).c_str(), glm::value_ptr(tmp));
		if (result)
		{
			Cocoa::CommandHistory::addCommand(new Cocoa::ChangeVec3Command(color, tmp));
		}
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			Cocoa::CommandHistory::setNoMergeMostRecent();
		}
		return result;
	}

	bool menuButton(const char* label, const glm::vec2& size)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
		bool res = ImGui::Button(label, ImVec2(size.x, size.y));
		ImGui::PopStyleVar();
		return res;
	}

	bool button(const char* label, const glm::vec2& size, bool invertTextColor)
	{
		if (invertTextColor)
		{
			ImVec4 color = ImGui::GetStyleColorVec4(ImGuiCol_TextInverted);
			ImGui::PushStyleColor(ImGuiCol_Text, color);
		}

		bool res = ImGui::Button(label, ImVec2(size.x, size.y));

		if (invertTextColor)
		{
			ImGui::PopStyleColor();
		}

		return res;
	}

	bool buttonDropdown(const char* label, const char* const items[], int items_count, int& item_pressed)
	{
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::GetStyleColorVec4(ImGuiCol_Button));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImGui::GetStyleColorVec4(ImGuiCol_ButtonHovered));
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
		ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextInverted));
		if (!ImGui::BeginCombo((std::string("##") + std::string(label)).c_str(), label, ImGuiComboFlags_NoArrowButton))
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
			const char* item_text = items[i];
			if (!item_text)
				item_text = "*Unknown item*";
			if (ImGui::Selectable(item_text, false))
			{
				value_changed = true;
				item_pressed = i;
			}
			ImGui::PopID();
		}

		ImGui::EndCombo();

		return value_changed;
	}

	void beginCollapsingHeaderGroup()
	{
		ImGui::BeginGroup();
		ImGui::Dummy(ImVec2(0, 10.0f));
		ImGui::Indent();
	}

	void endCollapsingHeaderGroup()
	{
		float windowWidth = ImGui::GetWindowSize().x;
		ImGui::Dummy(ImVec2(0, 10.0f));
		ImGui::Unindent();
		ImGui::EndGroup();
	}

	void readonlyText(const char* label, const std::string& readonlyTextValue)
	{
		ImGui::Text(label);
		char* buf = (char*)readonlyTextValue.c_str();
		int buf_size = readonlyTextValue.size();
		ImGui::InputText((std::string("##") + std::string(label)).c_str(), buf, buf_size, ImGuiInputTextFlags_ReadOnly);
	}

	bool undoableDragFloat4(const char* label, glm::vec4& vector)
	{
		glm::vec4 tmp = glm::vec4(vector);
		bool result = ImGui::DragFloat4(label, glm::value_ptr(tmp));
		if (result)
		{
			Cocoa::CommandHistory::addCommand(new Cocoa::ChangeVec4Command(vector, tmp));
		}
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			Cocoa::CommandHistory::setNoMergeMostRecent();
		}
		return result;
	}

	bool undoableDragFloat3(const char* label, glm::vec3& vector)
	{
		static int uid = 0;
		glm::vec3 tmp = glm::vec3(vector);
		ImGui::Text(label);

		if (ImGui::GetItemRectSize().x > textPadding)
		{
			textPadding = ImGui::GetItemRectSize().x;
		}

		ImGui::SameLine(textPadding);
		bool result = ImGui::DragFloat3((std::string("##") + std::string(label)).c_str(), glm::value_ptr(tmp));
		if (result)
		{
			Cocoa::CommandHistory::addCommand(new Cocoa::ChangeVec3Command(vector, tmp));
		}
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			Cocoa::CommandHistory::setNoMergeMostRecent();
		}
		return result;
	}

	bool undoableDragFloat2(const char* label, glm::vec2& vector)
	{
		glm::vec2 tmp = glm::vec2(vector);
		ImGui::Text(label);

		if (ImGui::GetItemRectSize().x > textPadding)
		{
			textPadding = ImGui::GetItemRectSize().x;
		}

		ImGui::SameLine(textPadding);
		bool result = ImGui::DragFloat2((std::string("##") + std::string(label)).c_str(), glm::value_ptr(tmp));
		if (result)
		{
			Cocoa::CommandHistory::addCommand(new Cocoa::ChangeVec2Command(vector, tmp));
		}
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			Cocoa::CommandHistory::setNoMergeMostRecent();
		}
		return result;
	}

	bool undoableDragFloat(const char* label, float& val)
	{
		float tmp = val;
		ImGui::Text(label);

		if (ImGui::GetItemRectSize().x > textPadding)
		{
			textPadding = ImGui::GetItemRectSize().x;
		}

		ImGui::SameLine(textPadding);
		bool result = ImGui::DragFloat((std::string("##") + std::string(label)).c_str(), &tmp);
		if (result)
		{
			Cocoa::CommandHistory::addCommand(new Cocoa::ChangeFloatCommand(val, tmp));
		}
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			Cocoa::CommandHistory::setNoMergeMostRecent();
		}
		return result;
	}

	bool undoableDragInt2(const char* label, glm::ivec2& vector)
	{
		glm::ivec2 tmp = glm::ivec2(vector);
		ImGui::Text(label);

		if (ImGui::GetItemRectSize().x > textPadding)
		{
			textPadding = ImGui::GetItemRectSize().x;
		}

		ImGui::SameLine(textPadding);
		bool result = ImGui::DragInt2((std::string("##") + std::string(label)).c_str(), glm::value_ptr(tmp));
		if (result)
		{
			Cocoa::CommandHistory::addCommand(new Cocoa::ChangeInt2Command(vector, tmp));
		}
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			Cocoa::CommandHistory::setNoMergeMostRecent();
		}
		return result;
	}

	bool undoableDragInt(const char* label, int& val)
	{
		int tmp = val;
		ImGui::Text(label);

		if (ImGui::GetItemRectSize().x > textPadding)
		{
			textPadding = ImGui::GetItemRectSize().x;
		}

		ImGui::SameLine(textPadding);
		bool result = ImGui::DragInt((std::string("##") + std::string(label)).c_str(), &tmp);
		if (result)
		{
			Cocoa::CommandHistory::addCommand(new Cocoa::ChangeIntCommand(val, tmp));
		}
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			Cocoa::CommandHistory::setNoMergeMostRecent();
		}
		return result;
	}

	bool imageButton(const Cocoa::Texture& texture, const glm::vec2& size, int framePadding,
		const glm::vec4& bgColor, const glm::vec4& tintColor)
	{
		return ImGui::ImageButton((ImTextureID)texture.graphicsId, { size.x, size.y }, ImVec2(0, 0), ImVec2(1, 1), framePadding, bgColor, tintColor);
	}

	bool inputText(const char* label, char* buf, size_t buf_size, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
	{
		return ImGui::InputText(label, buf, buf_size, flags, callback, user_data);
	}

	bool checkbox(const char* label, bool* checked)
	{
		return ImGui::Checkbox(label, checked);
	}

	ImVec4 from(const glm::vec4& vec4)
	{
		return ImVec4(vec4.x, vec4.y, vec4.z, vec4.w);
	}

	ImVec2 from(const glm::vec2& vec2)
	{
		return ImVec2(vec2.x, vec2.y);
	}

	bool beginDragDropTargetCurrentWindow()
	{
		ImGuiContext& g = *GImGui;
		if (!g.DragDropActive)
			return false;

		ImGuiWindow* window = g.CurrentWindow;
		if (window == NULL)
			return false;

		ImRect windowRect = window->Rect();
		if (!ImGui::IsMouseHoveringRect(windowRect.Min, windowRect.Max))
			return false;
		if (window->SkipItems)
			return false;

		IM_ASSERT(g.DragDropWithinTarget == false);
		g.DragDropTargetRect = windowRect;
		g.DragDropTargetId = window->ID;
		g.DragDropWithinTarget = true;
		return true;
	}
}