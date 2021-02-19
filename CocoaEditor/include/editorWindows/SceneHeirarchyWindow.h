#pragma once
#include "cocoa/core/Core.h"
#include "externalLibs.h"

#include "cocoa/scenes/SceneData.h"

namespace Cocoa
{
	namespace SceneHeirarchyWindow
	{
		void SetScene(SceneData& scene);

		void ImGui();
	};
}