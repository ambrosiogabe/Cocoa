#pragma once
#include "jade/renderer/Texture.h"

namespace Jade
{
	enum class AssetView
	{
		FileBrowser,
		TextureBrowser,
		Length
	};

	class AssetWindow
	{
	public:
		void ImGui();

	private:
		void ShowFileBrowser();
		void ShowMenuBar();
		void ShowTextureBrowser();
		bool IconButton(const char* icon, const char* label, const glm::vec2& size=glm::vec2());
		bool ImageButton(Texture* texture, const char* label, const glm::vec2& size=glm::vec2());

	private:
		glm::vec2 m_ButtonSize{ 128, 128 };
		AssetView m_CurrentView = AssetView::FileBrowser;
	};
}