#ifndef COCOA_ENGINE_JSON_EXTENDED_H
#define COCOA_ENGINE_JSON_EXTENDED_H
#include "externalLibs.h"
#include "cocoa/file/CPath.h"

namespace Cocoa
{
	namespace JsonExtended
	{
		COCOA void assignIfNotNull(const json& j, const char* property, uint8& val);
		COCOA void assignIfNotNull(const json& j, const char* property, uint16& val);
		COCOA void assignIfNotNull(const json& j, const char* property, uint32& val);
		COCOA void assignIfNotNull(const json& j, const char* property, int& val);
		COCOA void assignIfNotNull(const json& j, const char* property, float& val);
		COCOA void assignIfNotNull(const json& j, const char* property, bool& val);
		COCOA void assignIfNotNull(const json& j, const char* property, glm::vec2& vec);
		COCOA void assignIfNotNull(const json& j, const char* property, glm::vec3& vector);
		COCOA void assignIfNotNull(const json& j, const char* property, glm::vec4& vector);
		COCOA void assignIfNotNull(const json& j, const char* property, CPath& path);

		template<typename T>
		void assignEnumIfNotNull(const json& j, const char* property, T& enumValue)
		{
			if (j.contains(property) && j[property].is_number_integer())
			{
				int enumAsInt = (int)j[property];
				enumValue = (T)enumAsInt;
			}
		}
	}
}

#endif
