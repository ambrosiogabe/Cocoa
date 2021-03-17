#include "util/Settings.h"

namespace Cocoa
{
	namespace Settings
	{
		// TODO: Serialize this stuff on a per project basis or a global basis per user
		namespace Editor
		{
			// Which windows are open (of the windows that can be closed)
			bool ShowDemoWindow = false;
			bool ShowSettingsWindow = false;
			bool ShowStyleSelect = false;

			// Grid stuff
			bool SnapToGrid = false;
			bool DrawGrid = false;
			glm::vec2 GridSize = { 32.0f, 32.0f };
			glm::vec3 GridColor = { 0.1f, 0.1f, 0.1f };
			float GridStrokeWidth = 1.0f;

			// Selected Style
			int SelectedStyle = 0;
		}

		namespace EditorStyle
		{
			// Fonts
			ImFont* s_DefaultFont = nullptr;
			ImFont* s_LargeIconFont = nullptr;

			// Style Colors
			glm::vec4 s_MainBgLight0{ 0.404f, 0.404f ,0.404f, 1.0f };
			glm::vec4 s_MainBg{ 0.21f, 0.21f, 0.21f, 1.0f };
			glm::vec4 s_MainBgDark0{ 0.190f, 0.190f, 0.190f, 1.0f };
			glm::vec4 s_MainBgDark1{ 0.145f, 0.145f, 0.145f, 1.0f };
			glm::vec4 s_MainBgDark2{ 0.098f, 0.098f, 0.098f, 1.0f };

			glm::vec4 s_Accent{ 0.149f, 0.149f, 0.149f, 1.0f };
			glm::vec4 s_AccentDark0{ 0.102f, 0.102f, 0.102f, 1.0f };
			glm::vec4 s_AccentDark1{ 0.063f, 0.063f, 0.063f, 1.0f };

			glm::vec4 s_Button{ 0.882f, 0.882f, 0.882f, 1.0f };
			glm::vec4 s_ButtonHovered{ 0.782f, 0.782f, 0.782f, 1.0f };
			
			glm::vec4 s_Header{ 0.338f, 0.338f, 0.338f, 1.0f };
			glm::vec4 s_HeaderHovered{ 0.276f, 0.276f, 0.276f, 1.0f };
			glm::vec4 s_HeaderActive{ 0.379f, 0.379f, 0.379f, 1.0f };

			glm::vec4 s_Font{ 0.902f, 0.902f, 0.902f, 1.0f };
			glm::vec4 s_FontDisabled{ 0.36f, 0.36f, 0.36f, 1.0f };
			glm::vec4 s_HighlightColor{ 0.145f, 0.553f, 0.384f, 1.0f };

			// Sizing
			glm::vec2 s_WindowPadding{ 10, 10 };
			glm::vec2 s_FramePadding{ 20, 8 };
			glm::vec2 s_ItemSpacing{ 20, 8 };
			float s_ScrollbarSize = 17;
			float s_ScrollbarRounding = 12;
			float s_FrameRounding = 8;
			float s_GrabRounding = 8;
			float s_TabRounding = 8;
		}
	}
}