#pragma once
#include "cocoa/core/Core.h"
#include "externalLibs.h"

#include "cocoa/renderer/Texture.h"
#include "cocoa/scenes/SceneData.h"

namespace Cocoa
{
	namespace ProjectWizard
	{
		void Init();
		void ImGui(SceneData& scene);

		void CreateProjectImGui(SceneData& scene, bool& windowOpen);
	};
}