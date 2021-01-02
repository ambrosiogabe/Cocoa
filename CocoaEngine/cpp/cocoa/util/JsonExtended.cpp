#include "cocoa/util/JsonExtended.h"
#include "cocoa/util/CMath.h"

namespace Cocoa
{
	void JsonExtended::AssignIfNotNull(const json& j, glm::vec4& vector)
	{
		if (!j.is_null())
		{
			bool success = true;
			glm::vec4 tmp = CMath::DeserializeVec4(j, success);
			if (success)
			{
				vector = tmp;
			}
		}
	}

	void JsonExtended::AssignIfNotNull(const json& j, glm::vec3& vector)
	{
		if (!j.is_null())
		{
			bool success = true;
			glm::vec3 tmp = CMath::DeserializeVec3(j, success);
			if (success)
			{
				vector = tmp;
			}
		}
	}

	void JsonExtended::AssignIfNotNull(const json& j, glm::vec2& vector)
	{
		if (!j.is_null())
		{
			bool success = true;
			glm::vec2 tmp = CMath::DeserializeVec2(j, success);
			if (success)
			{
				vector = tmp;
			}
		}
	}

	void JsonExtended::AssignIfNotNull(const json& j, uint8& value)
	{
		if (!j.is_null())
		{
			value = j;
		}
	}

	void JsonExtended::AssignIfNotNull(const json& j, uint16& value)
	{
		if (!j.is_null())
		{
			value = j;
		}
	}

	void JsonExtended::AssignIfNotNull(const json& j, uint32& value)
	{
		if (!j.is_null())
		{
			value = j;
		}
	}

	void JsonExtended::AssignIfNotNull(const json& j, int& value)
	{
		if (!j.is_null())
		{
			value = j;
		}
	}

	void JsonExtended::AssignIfNotNull(const json& j, float& value)
	{
		if (!j.is_null())
		{
			value = j;
		}
	}
}