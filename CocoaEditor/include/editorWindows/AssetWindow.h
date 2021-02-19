#pragma once
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
		void ImGui(SceneData& scene);
	};
}