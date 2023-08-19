#include "externalLibs.h"

#include "cocoa/util/CMath.h"
#include "cocoa/util/JsonExtended.h"

#include <nlohmann/json.hpp>

namespace Cocoa
{
	namespace CMath
	{
		static float PI = 3.1415926535897932384626433832795028841971693993751058209749445923078164062f;

		bool compare(float x, float y, float epsilon)
		{
			return abs(x - y) <= epsilon * std::max(1.0f, std::max(abs(x), abs(y)));
		}

		bool compare(const glm::vec2& vec1, const glm::vec2& vec2, float epsilon)
		{
			return compare(vec1.x, vec2.x, epsilon) && compare(vec1.y, vec2.y, epsilon);
		}

		bool compare(const glm::vec3& vec1, const glm::vec3& vec2, float epsilon)
		{
			return compare(vec1.x, vec2.x, epsilon) && compare(vec1.y, vec2.y, epsilon) && compare(vec1.z, vec2.z, epsilon);
		}

		bool compare(const glm::vec4& vec1, const glm::vec4& vec2, float epsilon)
		{
			return compare(vec1.x, vec2.x, epsilon) && compare(vec1.y, vec2.y, epsilon) && compare(vec1.z, vec2.z, epsilon) && compare(vec1.w, vec2.w, epsilon);
		}

		glm::vec2 vector2From3(const glm::vec3& vec)
		{
			return glm::vec2(vec.x, vec.y);
		}

		glm::vec3 vector3From2(const glm::vec2& vec)
		{
			return glm::vec3(vec.x, vec.y, 0.0f);
		}

		float toRadians(float degrees)
		{
			return degrees * PI / 180.0f;
		}

		float toDegrees(float radians)
		{
			return radians * 180.0f / PI;
		}

		void rotate(glm::vec2& vec, float angleDeg, const glm::vec2& origin)
		{
			float x = vec.x - origin.x;
			float y = vec.y - origin.y;

			float xPrime = origin.x + ((x * (float)cos(toRadians(angleDeg))) - (y * (float)sin(toRadians(angleDeg))));
			float yPrime = origin.y + ((x * (float)sin(toRadians(angleDeg))) + (y * (float)cos(toRadians(angleDeg))));

			vec.x = xPrime;
			vec.y = yPrime;
		}

		void rotate(glm::vec3& vec, float angleDeg, const glm::vec3& origin)
		{
			// This function ignores Z values
			float x = vec.x - origin.x;
			float y = vec.y - origin.y;

			float xPrime = origin.x + ((x * (float)cos(toRadians(angleDeg))) - (y * (float)sin(toRadians(angleDeg))));
			float yPrime = origin.y + ((x * (float)sin(toRadians(angleDeg))) + (y * (float)cos(toRadians(angleDeg))));

			vec.x = xPrime;
			vec.y = yPrime;
		}

		json CMath::serialize(const std::string& name, const glm::vec4& vec)
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

		json CMath::serialize(const glm::vec4& vec)
		{
			return {
				{"X", vec.x},
				{"Y", vec.y},
				{"Z", vec.z},
				{"W", vec.w}
			};
		}

		glm::vec4 CMath::deserializeVec4(const json& j)
		{
			float x = 0.0f;
			JsonExtended::assignIfNotNull(j, "X", x);
			float y = 0.0f;
			JsonExtended::assignIfNotNull(j, "Y", y);
			float z = 0.0f;
			JsonExtended::assignIfNotNull(j, "Z", z);
			float w = 0.0f;
			JsonExtended::assignIfNotNull(j, "W", w);
			return glm::vec4{ x, y, z, w };
		}

		glm::vec4 CMath::deserializeVec4(const json& j, bool& success)
		{
			success = j.contains("X") && j.contains("Y") && j.contains("Z") && j.contains("W");
			return deserializeVec4(j);
		}

		json CMath::serialize(const std::string& name, const glm::vec3& vec)
		{
			return {
				name, {
					{"X", vec.x},
					{"Y", vec.y},
					{"Z", vec.z},
				}
			};
		}

		json CMath::serialize(const glm::vec3& vec)
		{
			return {
				{"X", vec.x},
				{"Y", vec.y},
				{"Z", vec.z}
			};
		}

		glm::vec3 CMath::deserializeVec3(const json& j)
		{
			float x = 0.0f;
			JsonExtended::assignIfNotNull(j, "X", x);
			float y = 0.0f;
			JsonExtended::assignIfNotNull(j, "Y", y);
			float z = 0.0f;
			JsonExtended::assignIfNotNull(j, "Z", z);
			return glm::vec3{ x, y, z };
		}

		glm::vec3 CMath::deserializeVec3(const json& j, bool& success)
		{
			success = j.contains("X") && j.contains("Y") && j.contains("Z");
			return deserializeVec3(j);
		}

		json CMath::serialize(const std::string& name, const glm::vec2& vec)
		{
			return {
				name, {
					{"X", vec.x},
					{"Y", vec.y},
				}
			};
		}

		json CMath::serialize(const glm::vec2& vec)
		{
			return {
				{"X", vec.x},
				{"Y", vec.y}
			};
		}

		glm::vec2 CMath::deserializeVec2(const json& j)
		{
			float x = 0.0f;
			JsonExtended::assignIfNotNull(j, "X", x);
			float y = 0.0f;
			JsonExtended::assignIfNotNull(j, "Y", y);
			return glm::vec2{ x, y };
		}

		glm::vec2 CMath::deserializeVec2(const json& j, bool& success)
		{
			success = j.contains("X") && j.contains("Y");
			return deserializeVec2(j);
		}

		float mapRange(float val, float inMin, float inMax, float outMin, float outMax)
		{
			return (val - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
		}

		int max(int a, int b)
		{
			return a > b ? a : b;
		}

		int min(int a, int b)
		{
			return a < b ? a : b;
		}

		float saturate(float val)
		{
			return val < 0 ? 0 :
				val > 1 ? 1 :
				val;
		}

		uint32 hashString(const char* str)
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