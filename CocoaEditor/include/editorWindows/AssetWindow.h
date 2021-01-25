#pragma once
#include "cocoa/core/Core.h"
#include "externalLibs.h"

#include "cocoa/renderer/Texture.h"

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

	class AssetWindow
	{
	public:
		AssetWindow(Scene* scene);
		void ImGui();

	private:
		void ShowMenuBar();
		void ShowTextureBrowser();
		void ShowSceneBrowser();
		void ShowScriptBrowser();
		void ShowFontBrowser();
		bool IconButton(const char* icon, const char* label, const glm::vec2& size=glm::vec2());
		bool ImageButton(const Texture& texture, const char* label, const glm::vec2& size=glm::vec2());

	private:
		glm::vec2 m_ButtonSize{ 128, 128 };
		AssetView m_CurrentView = AssetView::TextureBrowser;
		Scene* m_Scene;
	};
}