#include "cocoa/util/JsonExtended.h"
#include "cocoa/util/CMath.h"

namespace Cocoa
{
	void JsonExtended::AssignIfNotNull(const json& j, const char* property, glm::vec4& vector)
	{
		if (j.contains(property))
		{
			bool success = true;
			glm::vec4 tmp = CMath::DeserializeVec4(j[property], success);
			if (success)
			{
				vector = tmp;
			}
		}
	}

	void JsonExtended::AssignIfNotNull(const json& j, const char* property, glm::vec3& vector)
	{
		if (j.contains(property))
		{
			bool success = true;
			glm::vec3 tmp = CMath::DeserializeVec3(j[property], success);
			if (success)
			{
				vector = tmp;
			}
		}
	}

	void JsonExtended::AssignIfNotNull(const json& j, const char* property, glm::vec2& vector)
	{
		if (j.contains(property))
		{
			bool success = true;
			glm::vec2 tmp = CMath::DeserializeVec2(j[property], success);
			if (success)
			{
				vector = tmp;
			}
		}
	}

	void JsonExtended::AssignIfNotNull(const json& j, const char* property, uint8& value)
	{
		if (j.contains(property))
		{
			value = j[property];
		}
	}

	void JsonExtended::AssignIfNotNull(const json& j, const char* property, uint16& value)
	{
		if (j.contains(property))
		{
			value = j[property];
		}
	}

	void JsonExtended::AssignIfNotNull(const json& j, const char* property, uint32& value)
	{
		if (j.contains(property))
		{
			value = j[property];
		}
	}

	void JsonExtended::AssignIfNotNull(const json& j, const char* property, int& value)
	{
		if (j.contains(property))
		{
			value = j[property];
		}
	}

	void JsonExtended::AssignIfNotNull(const json& j, const char* property, float& value)
	{
		if (j.contains(property))
		{
			value = j[property];
		}
	}

	void JsonExtended::AssignIfNotNull(const json& j, const char* property, bool& value)
	{
		if (j.contains(property))
		{
			value = j[property];
		}
	}

	void JsonExtended::AssignIfNotNull(const json& j, const char* property, CPath& path)
	{
		if (j.contains(property) && j[property].is_string())
		{
			path = NCPath::CreatePath(std::string(j[property]));
		}
	}
}