#ifndef COCOA_EDITOR_GAME_EDITOR_VIEWPORT_H
#define COCOA_EDITOR_GAME_EDITOR_VIEWPORT_H
#include "cocoa/core/Core.h"
#include "externalLibs.h"

#include "cocoa/scenes/SceneData.h"

namespace Cocoa
{
	namespace GameEditorViewport
	{
		void imgui(SceneData& scene, bool* isWindowHovered);
	}
}

#endif
