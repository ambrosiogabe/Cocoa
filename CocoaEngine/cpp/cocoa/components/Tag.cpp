#include "cocoa/components/Tag.h"
#include "cocoa/util/JsonExtended.h"
#include "cocoa/core/Memory.h"

namespace Cocoa
{
	namespace NTag
	{
		Tag CreateTag(const char* name, bool isHeapAllocated)
		{
			Tag res;
			res.Name = name;
			res.Size = strlen(name);
			res.IsHeapAllocated = isHeapAllocated;
			res.HasChildren = false;
			res.Selected = false;
			return res;
		}

		void Destroy(Tag& tag)
		{
			if (tag.IsHeapAllocated)
			{
				FreeMem((void*)tag.Name);
				tag.Size = 0;
				tag.Name = nullptr;
			}
		}

		void ImGui()
		{

		}

		void Serialize(json& j, Entity entity, const Tag& tag)
		{
			int size = j["Components"].size();
			j["Components"][size] = {
				{"Tag", {
					{"Entity", NEntity::GetID(entity)},
					{"Name", tag.Name}
				}}
			};
		}
		void Deserialize(json& j, Entity entity)
		{
			Tag tag = { "", false, false };
			std::string tagName = j["Tag"]["Name"];
			int tagNameSize = tagName.length();
			if (tagNameSize > 0)
			{
				tag.Name = (char*)AllocMem(sizeof(char) * (tagNameSize + 1));
				memcpy((void*)tag.Name, tagName.c_str(), sizeof(char) * (tagNameSize + 1));
				tag.IsHeapAllocated = true;
			}

			NEntity::AddComponent<Tag>(entity, tag);
		}
	}
}