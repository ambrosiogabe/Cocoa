#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace ImGui {
	void UndoableColorEdit4(const char* label, glm::vec4& color);
	void UndoableColorEdit3(const char* label, glm::vec3& color);
	void UndoableDragFloat4(const char* label, glm::vec4& vector);
	void UndoableDragFloat3(const char* label, glm::vec3& vector);
	void UndoableDragFloat2(const char* label, glm::vec2& vector);
	void UndoableDragFloat(const char* label, float& val);
}