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

	void BeginCollapsingHeaderGroup(const ImVec2& itemRectMin, const ImVec2& itemRectMax)
	{
		ImGui::BeginGroup();
		ImGui::GetWindowDrawList()->AddRectFilled(itemRectMin, itemRectMax,
			ImGui::ColorConvertFloat4ToU32(From(Jade::Settings::EditorStyle::s_DarkInset)));
		ImGui::Dummy(ImVec2(0, 10.0f));
	}

	void EndCollapsingHeaderGroup(ImVec2& itemRectMin, ImVec2& itemRectMax)
	{
		float windowWidth = ImGui::GetWindowSize().x;
		ImGui::Dummy(ImVec2(0, 10.0f));
		ImGui::EndGroup();
		itemRectMin = ImGui::GetItemRectMin();
		itemRectMax = ImGui::GetItemRectMax() + ImVec2(windowWidth, 0);
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
}