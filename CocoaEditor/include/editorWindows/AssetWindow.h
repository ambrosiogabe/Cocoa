#ifndef COCOA_EDITOR_ASSET_WINDOW_H
#define COCOA_EDITOR_ASSET_WINDOW_H
#include "cocoa/core/Core.h"
#include "externalLibs.h"

#include "cocoa/scenes/SceneData.h"

namespace Cocoa
{
	enum class AssetView
	{
		TextureBrowser,
		SceneBrowser,
		ScriptBrowser,
		FontBrowser,
		Length
	};

	namespace AssetWindow
	{
		void imgui(SceneData& scene);
	};
}

#endif
