#include "util/Settings.h"

namespace Cocoa
{
	namespace Settings
	{
		// TODO: Serialize this stuff on a per project basis or a global basis per user
		namespace Editor
		{

			// Which windows are open (of the windows that can be closed)
			extern bool Editor::ShowDemoWindow = false;
			extern bool Editor::ShowSettingsWindow = false;
			extern bool Editor::ShowStyleSelect = false;

			// Grid stuff
			extern bool Editor::SnapToGrid = false;
			extern bool Editor::DrawGrid = false;
			extern glm::ivec2 Editor::GridSize = { 32, 32 };

			// Selected Style
			extern int Editor::SelectedStyle = 0;
		}

		namespace EditorStyle
		{
			// Fonts
			extern ImFont* EditorStyle::s_DefaultFont = nullptr;
			extern ImFont* EditorStyle::s_LargeIconFont = nullptr;

			// Style Colors
			extern glm::vec4 EditorStyle::s_MainBgLight0{ 0.404f, 0.404f ,0.404f, 1.0f };
			extern glm::vec4 EditorStyle::s_MainBg{ 0.212f, 0.212f, 0.212f, 1.0f };
			extern glm::vec4 EditorStyle::s_MainBgDark0{ 0.190f, 0.190f, 0.190f, 1.0f };
			extern glm::vec4 EditorStyle::s_MainBgDark1{ 0.145f, 0.145f, 0.145f, 1.0f };
			extern glm::vec4 EditorStyle::s_MainBgDark2{ 0.098f, 0.098f, 0.098f, 1.0f };

			extern glm::vec4 EditorStyle::s_Accent{ 0.149f, 0.149f, 0.149f, 1.0f };
			extern glm::vec4 EditorStyle::s_AccentDark0{ 0.102f, 0.102f, 0.102f, 1.0f };
			extern glm::vec4 EditorStyle::s_AccentDark1{ 0.063f, 0.063f, 0.063f, 1.0f };

			extern glm::vec4 EditorStyle::s_Button{ 0.882f, 0.882f, 0.882f, 1.0f };
			extern glm::vec4 EditorStyle::s_ButtonHovered{ 0.782f, 0.782f, 0.782f, 1.0f };

			extern glm::vec4 EditorStyle::s_Font{ 0.902f, 0.902f, 0.902f, 1.0f };
			extern glm::vec4 EditorStyle::s_FontDisabled{ 0.839f, 0.839f, 0.839f, 1.0f };
			extern glm::vec4 EditorStyle::s_HighlightColor{ 0.145f, 0.553f, 0.384f, 1.0f };

			// Sizing
			extern glm::vec2 EditorStyle::s_WindowPadding{ 10, 10 };
			extern glm::vec2 EditorStyle::s_FramePadding{ 20, 8 };
			extern glm::vec2 EditorStyle::s_ItemSpacing{ 20, 8 };
			extern float EditorStyle::s_ScrollbarSize = 17;
			extern float EditorStyle::s_ScrollbarRounding = 12;
			extern float EditorStyle::s_FrameRounding = 8;
			extern float EditorStyle::s_GrabRounding = 8;
			extern float EditorStyle::s_TabRounding = 8;
		}
	}
}