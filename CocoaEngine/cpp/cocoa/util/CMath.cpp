#include "externalLibs.h"

#include "cocoa/util/CMath.h"
#include "cocoa/util/JsonExtended.h"

#include <nlohmann/json.hpp>

namespace Cocoa
{
	namespace CMath
	{
		static float PI = 3.1415926535897932384626433832795028841971693993751058209749445923078164062f;

		bool Compare(float x, float y, float epsilon)
		{
			return abs(x - y) <= epsilon * std::max(1.0f, std::max(abs(x), abs(y)));
		}

		bool Compare(const glm::vec2& vec1, const glm::vec2& vec2, float epsilon)
		{
			return Compare(vec1.x, vec2.x, epsilon) && Compare(vec1.y, vec2.y, epsilon);
		}

		bool Compare(const glm::vec3& vec1, const glm::vec3& vec2, float epsilon)
		{
			return Compare(vec1.x, vec2.x, epsilon) && Compare(vec1.y, vec2.y, epsilon) && Compare(vec1.z, vec2.z, epsilon);
		}

		bool Compare(const glm::vec4& vec1, const glm::vec4& vec2, float epsilon)
		{
			return Compare(vec1.x, vec2.x, epsilon) && Compare(vec1.y, vec2.y, epsilon) && Compare(vec1.z, vec2.z, epsilon) && Compare(vec1.w, vec2.w, epsilon);
		}

		glm::vec2 Vector2From3(const glm::vec3& vec)
		{
			return glm::vec2(vec.x, vec.y);
		}

		glm::vec3 Vector3From2(const glm::vec2& vec)
		{
			return glm::vec3(vec.x, vec.y, 0.0f);
		}

		float ToRadians(float degrees)
		{
			return degrees * PI / 180.0f;
		}

		float ToDegrees(float radians)
		{
			return radians * 180.0f / PI;
		}

		void Rotate(glm::vec2& vec, float angleDeg, const glm::vec2& origin)
		{
			float x = vec.x - origin.x;
			float y = vec.y - origin.y;

			float xPrime = origin.x + ((x * (float)cos(ToRadians(angleDeg))) - (y * (float)sin(ToRadians(angleDeg))));
			float yPrime = origin.y + ((x * (float)sin(ToRadians(angleDeg))) + (y * (float)cos(ToRadians(angleDeg))));

			vec.x = xPrime;
			vec.y = yPrime;
		}

		void Rotate(glm::vec3& vec, float angleDeg, const glm::vec3& origin)
		{
			// This function ignores Z values
			float x = vec.x - origin.x;
			float y = vec.y - origin.y;

			float xPrime = origin.x + ((x * (float)cos(ToRadians(angleDeg))) - (y * (float)sin(ToRadians(angleDeg))));
			float yPrime = origin.y + ((x * (float)sin(ToRadians(angleDeg))) + (y * (float)cos(ToRadians(angleDeg))));

			vec.x = xPrime;
			vec.y = yPrime;
		}

		json CMath::Serialize(const std::string& name, const glm::vec4& vec)
		{
			return {
				name, {
					{"X", vec.x},
					{"Y", vec.y},
					{"Z", vec.z},
					{"W", vec.w}
           		}
			};
		}

		json CMath::Serialize(const glm::vec4& vec)
		{
			return {
				{"X", vec.x},
				{"Y", vec.y},
				{"Z", vec.z},
				{"W", vec.w}
			};
		}

		glm::vec4 CMath::DeserializeVec4(const json& j)
		{
			float x = 0.0f;
			JsonExtended::AssignIfNotNull(j, "X", x);
			float y = 0.0f;
			JsonExtended::AssignIfNotNull(j, "Y", y);
			float z = 0.0f;
			JsonExtended::AssignIfNotNull(j, "Z", z);
			float w = 0.0f;
			JsonExtended::AssignIfNotNull(j, "W", w);
			return glm::vec4{ x, y, z, w };
		}

		glm::vec4 CMath::DeserializeVec4(const json& j, bool& success)
		{
			success = j.contains("X") && j.contains("Y") && j.contains("Z") && j.contains("W");
			return DeserializeVec4(j);
		}

		json CMath::Serialize(const std::string& name, const glm::vec3& vec)
		{
			return {
				name, {
					{"X", vec.x},
					{"Y", vec.y},
					{"Z", vec.z},
				}
			};
		}

		json CMath::Serialize(const glm::vec3& vec)
		{
			return {
				{"X", vec.x},
				{"Y", vec.y},
				{"Z", vec.z}
			};
		}

		glm::vec3 CMath::DeserializeVec3(const json& j)
		{
			float x = 0.0f;
			JsonExtended::AssignIfNotNull(j, "X", x);
			float y = 0.0f;
			JsonExtended::AssignIfNotNull(j, "Y", y);
			float z = 0.0f;
			JsonExtended::AssignIfNotNull(j, "Z", z);
			return glm::vec3{ x, y, z };
		}

		glm::vec3 CMath::DeserializeVec3(const json& j, bool& success)
		{
			success = j.contains("X") && j.contains("Y") && j.contains("Z");
			return DeserializeVec3(j);
		}

		json CMath::Serialize(const std::string& name, const glm::vec2& vec)
		{
			return {
				name, {
					{"X", vec.x},
					{"Y", vec.y},
				}
			};
		}

		json CMath::Serialize(const glm::vec2& vec)
		{
			return {
				{"X", vec.x},
				{"Y", vec.y}
			};
		}

		glm::vec2 CMath::DeserializeVec2(const json& j)
		{
			float x = 0.0f;
			JsonExtended::AssignIfNotNull(j, "X", x);
			float y = 0.0f;
			JsonExtended::AssignIfNotNull(j, "Y", y);
			return glm::vec2{ x, y };
		}

		glm::vec2 CMath::DeserializeVec2(const json& j, bool& success)
		{
			success = j.contains("X") && j.contains("Y");
			return DeserializeVec2(j);
		}

		float MapRange(float val, float inMin, float inMax, float outMin, float outMax)
		{
			return (val - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
		}

		int Max(int a, int b)
		{
			return a > b ? a : b;
		}

		int Min(int a, int b)
		{
			return a < b ? a : b;
		}

		float Saturate(float val)
		{
			return val < 0 ? 0 :
				val > 1 ? 1 :
				val;
		}

		uint32 HashString(const char* str)
		{
			uint32 hash = 2166136261u;
			int length = strlen(str);

			for (int i = 0; i < length; i++)
			{
				hash ^= str[i];
				hash *= 16777619;
			}

			return hash;
		}
	}
}