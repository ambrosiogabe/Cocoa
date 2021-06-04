#include "cocoa/components/Tag.h"
#include "cocoa/util/JsonExtended.h"

namespace Cocoa
{
	namespace NTag
	{
		Tag createTag(const char* name, bool isHeapAllocated)
		{
			Tag res;
			res.name = name;
			res.size = strlen(name);
			res.isHeapAllocated = isHeapAllocated;
			return res;
		}

		void destroy(Tag& tag)
		{
			if (tag.isHeapAllocated)
			{
				FreeMem((void*)tag.name);
				tag.size = 0;
				tag.name = nullptr;
			}
		}

		void ImGui()
		{

		}

		void serialize(json& j, Entity entity, const Tag& tag)
		{
			int size = j["Components"].size();
			j["Components"][size] = {
				{"Tag", {
					{"Entity", NEntity::GetID(entity)},
					{"Name", tag.name}
				}}
			};
		}
		void deserialize(const json& j, Entity entity)
		{
			Tag tag = { "", false, false };
			std::string tagName = j["Tag"]["Name"];

			int tagNameSize = tagName.length();
			if (tagNameSize > 0)
			{
				tag.name = (char*)AllocMem(sizeof(char) * (tagNameSize + 1));
				memcpy((void*)tag.name, tagName.c_str(), sizeof(char) * (tagNameSize + 1));
				tag.isHeapAllocated = true;
				tag.size = tagNameSize;
			}

			NEntity::AddComponent<Tag>(entity, tag);
		}
	}
}