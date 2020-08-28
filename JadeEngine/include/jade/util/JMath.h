#pragma once
#include "externalLibs.h"

namespace Jade
{
	namespace JMath
	{
		// Float Comparison functions, using custom epsilon
		JADE bool Compare(float x, float y, float epsilon = std::numeric_limits<float>::min());
		JADE bool Compare(const glm::vec3& vec1, const glm::vec3& vec2, float epsilon = std::numeric_limits<float>::min());
		JADE bool Compare(const glm::vec2& vec1, const glm::vec2& vec2, float epsilon = std::numeric_limits<float>::min());
		JADE bool Compare(const glm::vec4& vec1, const glm::vec4& vec2, float epsilon = std::numeric_limits<float>::min());

		// Vector conversions
		JADE glm::vec2 Vector2From3(const glm::vec3& vec);
		JADE glm::vec3 Vector3From2(const glm::vec2& vec);

		// Math functions
		JADE void Rotate(glm::vec2& vec, float angleDeg, const glm::vec2& origin);
		JADE void Rotate(glm::vec3& vec, float angleDeg, const glm::vec3& origin);

		JADE float ToRadians(float degrees);
		JADE float ToDegrees(float radians);

		// Serialize math components
		JADE json Serialize(const std::string& name, const glm::vec4& vec);
		JADE glm::vec4 DeserializeVec4(const json& json);
		JADE glm::vec4 DeserializeVec4(const json& json, bool& success);
		JADE json Serialize(const std::string& name, const glm::vec3& vec);
		JADE glm::vec3 DeserializeVec3(const json& json);
		JADE glm::vec3 DeserializeVec3(const json& json, bool& success);
		JADE json Serialize(const std::string& name, const glm::vec2& vec);
		JADE glm::vec2 DeserializeVec2(const json& json);
		JADE glm::vec2 DeserializeVec2(const json& json, bool& success);
	}
}