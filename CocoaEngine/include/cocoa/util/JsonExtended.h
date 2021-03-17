#pragma once
#include "externalLibs.h"
#include "cocoa/file/CPath.h"

namespace Cocoa
{
	namespace JsonExtended
	{
		COCOA void AssignIfNotNull(const json& j, const char* property, uint8& val);
		COCOA void AssignIfNotNull(const json& j, const char* property, uint16& val);
		COCOA void AssignIfNotNull(const json& j, const char* property, uint32& val);
		COCOA void AssignIfNotNull(const json& j, const char* property, int& val);
		COCOA void AssignIfNotNull(const json& j, const char* property, float& val);
		COCOA void AssignIfNotNull(const json& j, const char* property, bool& val);
		COCOA void AssignIfNotNull(const json& j, const char* property, glm::vec2& vec);
		COCOA void AssignIfNotNull(const json& j, const char* property, glm::vec3& vector);
		COCOA void AssignIfNotNull(const json& j, const char* property, glm::vec4& vector);
		COCOA void AssignIfNotNull(const json& j, const char* property, CPath& path);

		template<typename T>
		void AssignEnumIfNotNull(const json& j, const char* property, T& enumValue)
		{
			if (j.contains(property) && j[property].is_number_integer())
			{
				int enumAsInt = (int)j[property];
				enumValue = (T)enumAsInt;
			}
		}
	}
}