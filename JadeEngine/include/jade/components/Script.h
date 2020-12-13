#pragma once
#include "jade/core/Core.h"
#include "externalLibs.h"
#include "jade/core/Entity.h"

namespace Jade
{
	class JADE Script
	{
	public:
		virtual void Update(float dt) {}
		virtual void ImGui() {}

		//virtual void BaseImGui() = 0;
		//virtual void Serialize(json& j, Entity entity, const Script& script) = 0;
		//virtual void DeSerialize(json& j, Entity entity) = 0;
	};
}