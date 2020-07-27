#include "jade/util/Settings.h"
#include "jade/commands/ICommand.h"

#include <imgui.h>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace ImGui {
	ImVec4 From(const glm::vec4& vec4);

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
		glm::vec4 darkInset = Jade::Settings::EditorStyle::s_DarkBgColor;
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(darkInset.x, darkInset.y, darkInset.z, darkInset.w));
		if (ImGui::Combo(label, &current_item, items, items_count))
		{
			Jade::CommandHistory::AddCommand(new Jade::ChangeEnumCommand<T>(enumVal, static_cast<T>(current_item)));
			Jade::CommandHistory::SetNoMergeMostRecent();
			return true;
		}
		ImGui::PopStyleColor();
		return false;
	}

	void BeginCollapsingHeaderGroup(const ImVec2& itemRectMin, const ImVec2& itemRectMax);
	void EndCollapsingHeaderGroup(ImVec2& itemRectMin, ImVec2& itemRectMax);
}