#pragma once
#include "cocoa/core/Core.h"
#include "externalLibs.h"

#include "cocoa/scenes/SceneData.h"

namespace Cocoa
{
	namespace GameEditorViewport
	{
		void ImGui(SceneData& scene, bool* isWindowHovered);
	}
}