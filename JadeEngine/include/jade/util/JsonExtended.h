#pragma once
#include "externalLibs.h"


namespace Jade
{
	namespace JsonExtended
	{
		void AssignIfNotNull(const json& j, uint8& val);
		void AssignIfNotNull(const json& j, uint16& val);
		void AssignIfNotNull(const json& j, uint32& val);
		void AssignIfNotNull(const json& j, int& val);
		void AssignIfNotNull(const json& j, float& val);
		void AssignIfNotNull(const json& j, glm::vec2& vec);
		void AssignIfNotNull(const json& j, glm::vec3& vector);
		void AssignIfNotNull(const json& j, glm::vec4& vector);
	}
}