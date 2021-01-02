#pragma once
#include "cocoa/core/Core.h"

namespace Cocoa
{
	class Script
	{
	public:
		virtual void Start() {}
		virtual void EditorUpdate(float dt) {}
		virtual void Update(float dt) {}
		virtual void ImGui() {}

		//virtual void BaseImGui() = 0;
		//virtual void Serialize(json& j, Entity entity, const Script& script) = 0;
		//virtual void DeSerialize(json& j, Entity entity) = 0;
	};
}