#pragma once
#include "cocoa/core/Core.h"
#include "externalLibs.h"

namespace Cocoa
{
	class Scene;

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
		void ImGui(Scene* scene);
	};
}