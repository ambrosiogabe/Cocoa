#ifndef COCOA_ENGINE_CMATH_H
#define COCOA_ENGINE_CMATH_H
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
		COCOA bool compare(float x, float y, float epsilon = std::numeric_limits<float>::min());
		COCOA bool compare(const glm::vec3& vec1, const glm::vec3& vec2, float epsilon = std::numeric_limits<float>::min());
		COCOA bool compare(const glm::vec2& vec1, const glm::vec2& vec2, float epsilon = std::numeric_limits<float>::min());
		COCOA bool compare(const glm::vec4& vec1, const glm::vec4& vec2, float epsilon = std::numeric_limits<float>::min());

		// Vector conversions
		COCOA glm::vec2 vector2From3(const glm::vec3& vec);
		COCOA glm::vec3 vector3From2(const glm::vec2& vec);

		// Math functions
		COCOA void rotate(glm::vec2& vec, float angleDeg, const glm::vec2& origin);
		COCOA void rotate(glm::vec3& vec, float angleDeg, const glm::vec3& origin);

		COCOA float toRadians(float degrees);
		COCOA float toDegrees(float radians);

		// Serialize math components
		COCOA json serialize(const std::string& name, const glm::vec4& vec);
		COCOA json serialize(const glm::vec4& vec);
		COCOA glm::vec4 deserializeVec4(const json& json);
		COCOA glm::vec4 deserializeVec4(const json& json, bool& success);
		COCOA json serialize(const std::string& name, const glm::vec3& vec);
		COCOA json serialize(const glm::vec3& vec);
		COCOA glm::vec3 deserializeVec3(const json& json);
		COCOA glm::vec3 deserializeVec3(const json& json, bool& success);
		COCOA json serialize(const std::string& name, const glm::vec2& vec);
		COCOA json serialize(const glm::vec2& vec);
		COCOA glm::vec2 deserializeVec2(const json& json);
		COCOA glm::vec2 deserializeVec2(const json& json, bool& success);

		// Map Ranges
		COCOA float mapRange(float val, float inMin, float inMax, float outMin, float outMax);

		// Max, Min impls
		COCOA int max(int a, int b);
		COCOA int min(int a, int b);
		COCOA float saturate(float val);

		// Hash Strings
		COCOA uint32 hashString(const char* str);
	}
}

#endif
