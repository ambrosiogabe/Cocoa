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

	struct AssetWindow
	{
		Scene* ScenePtr = nullptr;
		AssetView CurrentView = AssetView::TextureBrowser;;
	};

	namespace AssetWindowUtil
	{
		void ImGui(AssetWindow& assetWindow);
	};
}