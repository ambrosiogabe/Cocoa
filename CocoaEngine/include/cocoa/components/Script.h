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

		virtual void BeginContact(Entity other) {}
		virtual void EndContact(Entity other) {}
	};
}