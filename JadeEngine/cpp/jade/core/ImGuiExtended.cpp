#include "externalLibs.h"
#include "jade/core/ImGuiExtended.h"
#include "jade/core/Core.h"

namespace ImGui
{
	ImVec4 From(const glm::vec4& vec4)
	{
		return ImVec4(vec4.x, vec4.y, vec4.z, vec4.w);
	}

	static float textPadding = 0;
	void UndoableColorEdit4(const char* label, glm::vec4& color)
	{
		glm::vec4 tmp = glm::vec4(color);
		ImGui::Text(label);

		if (ImGui::GetItemRectSize().x > textPadding)
		{
			textPadding = ImGui::GetItemRectSize().x;
		}

		ImGui::SameLine(textPadding);
		if (ImGui::ColorEdit4((std::string("##") + std::string(label)).c_str(), glm::value_ptr(tmp)))
		{
			Jade::CommandHistory::AddCommand(new Jade::ChangeVec4Command(color, tmp));
		}
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			Jade::CommandHistory::SetNoMergeMostRecent();
		}
	}

	void UndoableColorEdit3(const char* label, glm::vec3& color)
	{
		glm::vec3 tmp = glm::vec3(color);
		ImGui::Text(label);

		if (ImGui::GetItemRectSize().x > textPadding)
		{
			textPadding = ImGui::GetItemRectSize().x;
		}

		ImGui::SameLine(textPadding);
		if (ImGui::ColorEdit3((std::string("##") + std::string(label)).c_str(), glm::value_ptr(tmp)))
		{
			Jade::CommandHistory::AddCommand(new Jade::ChangeVec3Command(color, tmp));
		}
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			Jade::CommandHistory::SetNoMergeMostRecent();
		}
	}

	bool MenuButton(const char* label, const glm::vec2& size)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
		bool res = ImGui::Button(label, ImVec2(size.x, size.y));
		ImGui::PopStyleVar();
		return res;
	}

	bool JButton(const char* label, const glm::vec2& size)
	{
		ImVec4 color = ImGui::GetStyleColorVec4(ImGuiCol_TextInverted);
		ImGui::PushStyleColor(ImGuiCol_Text, color);
		bool res = ImGui::Button(label, ImVec2(size.x, size.y));
		ImGui::PopStyleColor();
		return res;
	}

	bool JButtonDropdown(const char* label, const char* const items[], int items_count, int& item_pressed)
	{
		ImGui::PushStyleColor(ImGuiCol_FrameBg, GetStyleColorVec4(ImGuiCol_Button));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, GetStyleColorVec4(ImGuiCol_ButtonHovered));
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, GetStyleColorVec4(ImGuiCol_ButtonActive));
		ImGui::PushStyleColor(ImGuiCol_Text, GetStyleColorVec4(ImGuiCol_TextInverted));
		if (!BeginCombo((std::string("##") + std::string(label)).c_str(), label, ImGuiComboFlags_NoArrowButton))
		{
			ImGui::PopStyleColor(4);
			return false;
		}
		ImGui::PopStyleColor(4);

		// Display items
		bool value_changed = false;
		for (int i = 0; i < items_count; i++)
		{
			PushID((void*)(intptr_t)i);
			const char* item_text = items[i];
			if (!item_text)
				item_text = "*Unknown item*";
			if (Selectable(item_text, false))
			{
				value_changed = true;
				item_pressed = i;
			}
			PopID();
		}

		EndCombo();

		return value_changed;
	}

	void BeginCollapsingHeaderGroup()
	{
		ImGui::BeginGroup();
		ImGui::Dummy(ImVec2(0, 10.0f));
		ImGui::Indent();
	}

	void EndCollapsingHeaderGroup()
	{
		float windowWidth = ImGui::GetWindowSize().x;
		ImGui::Dummy(ImVec2(0, 10.0f));
		ImGui::Unindent();
		ImGui::EndGroup();
	}

	void UndoableDragFloat4(const char* label, glm::vec4& vector)
	{
		glm::vec4 tmp = glm::vec4(vector);
		if (ImGui::DragFloat4(label, glm::value_ptr(tmp)))
		{
			Jade::CommandHistory::AddCommand(new Jade::ChangeVec4Command(vector, tmp));
		}
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			Jade::CommandHistory::SetNoMergeMostRecent();
		}
	}

	void UndoableDragFloat3(const char* label, glm::vec3& vector)
	{
		static int uid = 0;
		glm::vec3 tmp = glm::vec3(vector);
		ImGui::Text(label);

		if (ImGui::GetItemRectSize().x > textPadding)
		{
			textPadding = ImGui::GetItemRectSize().x;
		}

		ImGui::SameLine(textPadding);
		if (ImGui::DragFloat3((std::string("##") + std::string(label)).c_str(), glm::value_ptr(tmp)))
		{
			Jade::CommandHistory::AddCommand(new Jade::ChangeVec3Command(vector, tmp));
		}
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			Jade::CommandHistory::SetNoMergeMostRecent();
		}
	}

	void UndoableDragFloat2(const char* label, glm::vec2& vector)
	{
		glm::vec2 tmp = glm::vec2(vector);
		ImGui::Text(label);
		
		if (ImGui::GetItemRectSize().x > textPadding)
		{
			textPadding = ImGui::GetItemRectSize().x;
		}

		ImGui::SameLine(textPadding);
		if (ImGui::DragFloat2((std::string("##") + std::string(label)).c_str(), glm::value_ptr(tmp)))
		{
			Jade::CommandHistory::AddCommand(new Jade::ChangeVec2Command(vector, tmp));
		}
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			Jade::CommandHistory::SetNoMergeMostRecent();
		}
	}

	void UndoableDragFloat(const char* label, float& val)
	{
		float tmp = val;
		ImGui::Text(label);
		
		if (ImGui::GetItemRectSize().x > textPadding)
		{
			textPadding = ImGui::GetItemRectSize().x;
		}

		ImGui::SameLine(textPadding);
		if (ImGui::DragFloat((std::string("##") + std::string(label)).c_str(), &tmp))
		{
			Jade::CommandHistory::AddCommand(new Jade::ChangeFloatCommand(val, tmp));
		}
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			Jade::CommandHistory::SetNoMergeMostRecent();
		}
	}

	bool JImageButton(const Jade::Texture& texture, const glm::vec2& size, int framePadding,
		const glm::vec4& bgColor, const glm::vec4& tintColor)
	{
		return ImGui::ImageButton((ImTextureID)texture.GetId(), { size.x, size.y }, ImVec2(0, 0), ImVec2(1, 1), framePadding,
			ImGui::From(bgColor), ImGui::From(tintColor));
	}
}