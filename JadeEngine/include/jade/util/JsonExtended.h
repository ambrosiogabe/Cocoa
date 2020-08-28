#pragma once
#include "externalLibs.h"


namespace Jade
{
	namespace JsonExtended
	{
		JADE void AssignIfNotNull(const json& j, uint8& val);
		JADE void AssignIfNotNull(const json& j, uint16& val);
		JADE void AssignIfNotNull(const json& j, uint32& val);
		JADE void AssignIfNotNull(const json& j, int& val);
		JADE void AssignIfNotNull(const json& j, float& val);
		JADE void AssignIfNotNull(const json& j, glm::vec2& vec);
		JADE void AssignIfNotNull(const json& j, glm::vec3& vector);
		JADE void AssignIfNotNull(const json& j, glm::vec4& vector);
	}
}