#pragma once
#include "externalLibs.h"

// TODO: Find who's leaking min, max macros...
#ifdef min 
#undef min
#endif
#ifdef max
#undef max
#endif

namespace Cocoa
{
	namespace CMath
	{
		// Float Comparison functions, using custom epsilon
		COCOA bool Compare(float x, float y, float epsilon = std::numeric_limits<float>::min());
		COCOA bool Compare(const glm::vec3& vec1, const glm::vec3& vec2, float epsilon = std::numeric_limits<float>::min());
		COCOA bool Compare(const glm::vec2& vec1, const glm::vec2& vec2, float epsilon = std::numeric_limits<float>::min());
		COCOA bool Compare(const glm::vec4& vec1, const glm::vec4& vec2, float epsilon = std::numeric_limits<float>::min());

		// Vector conversions
		COCOA glm::vec2 Vector2From3(const glm::vec3& vec);
		COCOA glm::vec3 Vector3From2(const glm::vec2& vec);

		// Math functions
		COCOA void Rotate(glm::vec2& vec, float angleDeg, const glm::vec2& origin);
		COCOA void Rotate(glm::vec3& vec, float angleDeg, const glm::vec3& origin);

		COCOA float ToRadians(float degrees);
		COCOA float ToDegrees(float radians);

		// Serialize math components
		COCOA json Serialize(const std::string& name, const glm::vec4& vec);
		COCOA glm::vec4 DeserializeVec4(const json& json);
		COCOA glm::vec4 DeserializeVec4(const json& json, bool& success);
		COCOA json Serialize(const std::string& name, const glm::vec3& vec);
		COCOA glm::vec3 DeserializeVec3(const json& json);
		COCOA glm::vec3 DeserializeVec3(const json& json, bool& success);
		COCOA json Serialize(const std::string& name, const glm::vec2& vec);
		COCOA glm::vec2 DeserializeVec2(const json& json);
		COCOA glm::vec2 DeserializeVec2(const json& json, bool& success);

		// Map Ranges
		COCOA float MapRange(float val, float inMin, float inMax, float outMin, float outMax);

		// Max, Min impls
		COCOA int Max(int a, int b);
		COCOA int Min(int a, int b);
	}
}