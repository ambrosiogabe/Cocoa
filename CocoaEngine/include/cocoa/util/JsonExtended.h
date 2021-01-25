#pragma once
#include "externalLibs.h"
#include "cocoa/file/CPath.h"

namespace Cocoa
{
	namespace JsonExtended
	{
		COCOA void AssignIfNotNull(const json& j, uint8& val);
		COCOA void AssignIfNotNull(const json& j, uint16& val);
		COCOA void AssignIfNotNull(const json& j, uint32& val);
		COCOA void AssignIfNotNull(const json& j, int& val);
		COCOA void AssignIfNotNull(const json& j, float& val);
		COCOA void AssignIfNotNull(const json& j, glm::vec2& vec);
		COCOA void AssignIfNotNull(const json& j, glm::vec3& vector);
		COCOA void AssignIfNotNull(const json& j, glm::vec4& vector);
		COCOA void AssignIfNotNull(const json& j, CPath& path);
	}
}