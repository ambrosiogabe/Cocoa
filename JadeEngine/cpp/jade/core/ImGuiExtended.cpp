#include "jade/core/ImGuiExtended.h"
#include "jade/commands/CommandHistory.h"
#include "jade/commands/ChangeFloatCommand.h"
#include "jade/commands/ChangeVec2Command.h"
#include "jade/commands/ChangeVec3Command.h"
#include "jade/commands/ChangeVec4Command.h"

#pragma warning( push )
#pragma warning ( disable : 26812 )
#pragma warning ( disable : 26451 )
#pragma warning ( disable : 26495 )
#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui_internal.h>
#pragma warning( pop )
#undef IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imstb_rectpack.h>

#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace ImGui
{
	void UndoableColorEdit4(const char* label, glm::vec4& color)
	{
		glm::vec4 tmp = glm::vec4(color);
		if (ImGui::ColorEdit4(label, glm::value_ptr(tmp)))
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
		if (ImGui::ColorEdit3(label, glm::value_ptr(tmp)))
		{
			Jade::CommandHistory::AddCommand(new Jade::ChangeVec3Command(color, tmp));
		}
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			Jade::CommandHistory::SetNoMergeMostRecent();
		}
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
		glm::vec3 tmp = glm::vec3(vector);
		if (ImGui::DragFloat3(label, glm::value_ptr(tmp)))
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
		if (ImGui::DragFloat2(label, glm::value_ptr(tmp)))
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
		if (ImGui::DragFloat(label, &tmp))
		{
			Jade::CommandHistory::AddCommand(new Jade::ChangeFloatCommand(val, tmp));
		}
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			Jade::CommandHistory::SetNoMergeMostRecent();
		}
	}
}