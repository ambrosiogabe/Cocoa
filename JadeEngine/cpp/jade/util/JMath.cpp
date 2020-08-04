#include "externalLibs.h"

#include "jade/util/JMath.h"

#include <nlohmann/json.hpp>

namespace Jade
{
	namespace JMath
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

		json JMath::Serialize(const std::string& name, const glm::vec4& vec)
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

		glm::vec4 JMath::DeserializeVec4(json& j)
		{
			float x = j["X"].is_null() ? 0.0f : j["X"];
			float y = j["Y"].is_null() ? 0.0f : j["Y"];
			float z = j["Z"].is_null() ? 0.0f : j["Z"];
			float w = j["W"].is_null() ? 0.0f : j["W"];
			return glm::vec4{ x, y, z, w };
		}

		glm::vec4 JMath::DeserializeVec4(json& j, bool& success)
		{
			success = !j["X"].is_null() && !j["Y"].is_null() && !j["Z"].is_null() && !j["W"].is_null();
			return DeserializeVec4(j);
		}

		json JMath::Serialize(const std::string& name, const glm::vec3& vec)
		{
			return {
				name, {
					{"X", vec.x},
					{"Y", vec.y},
					{"Z", vec.z},
				}
			};
		}

		glm::vec3 JMath::DeserializeVec3(json& j)
		{
			float x = j["X"].is_null() ? 0.0f : j["X"];
			float y = j["Y"].is_null() ? 0.0f : j["Y"];
			float z = j["Z"].is_null() ? 0.0f : j["Z"];
			return glm::vec3{ x, y, z };
		}

		glm::vec3 JMath::DeserializeVec3(json& j, bool& success)
		{
			success = !j["X"].is_null() && !j["Y"].is_null() && !j["Z"].is_null();
			return DeserializeVec3(j);
		}

		json JMath::Serialize(const std::string& name, const glm::vec2& vec)
		{
			return {
				name, {
					{"X", vec.x},
					{"Y", vec.y},
				}
			};
		}

		glm::vec2 JMath::DeserializeVec2(json& j)
		{
			float x = j["X"].is_null() ? 0.0f : j["X"];
			float y = j["Y"].is_null() ? 0.0f : j["Y"];
			return glm::vec2{ x, y };
		}

		glm::vec2 JMath::DeserializeVec2(json& j, bool& success)
		{
			success = !j["X"].is_null() && !j["Y"].is_null();
			return DeserializeVec2(j);
		}
	}
}