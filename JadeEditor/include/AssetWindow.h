#pragma once
#include <imgui/imgui.h>

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
		bool IconButton(const char* icon, const char* label, const ImVec2& size=ImVec2(0, 0));

	private:
		ImVec2 m_ButtonSize{ 128, 128 };
		AssetView m_CurrentView = AssetView::FileBrowser;
	};
}