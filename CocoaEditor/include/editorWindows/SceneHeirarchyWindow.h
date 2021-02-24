#pragma once
#include "cocoa/core/Core.h"
#include "externalLibs.h"

#include "cocoa/scenes/SceneData.h"

namespace Cocoa
{
	namespace SceneHeirarchyWindow
	{
		void Init();
		void Destroy();

		void AddNewEntity(Entity entity);
		void ImGui(SceneData& scene);

		void Serialize(json& j);
		void Deserialize(json& json, SceneData& scene);
	};
}