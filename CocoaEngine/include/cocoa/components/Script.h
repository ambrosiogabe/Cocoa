#pragma once
#include "cocoa/core/Core.h"
#include "cocoa/core/EntityStruct.h"

namespace Cocoa
{
	class Script
	{
	public:
		virtual void Start(Entity entity) {}
		virtual void EditorUpdate(Entity entity, float dt) {}
		virtual void Update(Entity entity, float dt) {}
		virtual void ImGui(Entity entity) {}

		//virtual void BaseImGui() = 0;
		//virtual void Serialize(json& j, Entity entity, const Script& script) = 0;
		//virtual void DeSerialize(json& j, Entity entity) = 0;
	};
}