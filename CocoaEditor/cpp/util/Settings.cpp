#include "util/Settings.h"

namespace Cocoa
{
	namespace Settings
	{
		// TODO: Serialize this stuff on a per project basis or a global basis per user
		namespace Editor
		{
			// Which windows are open (of the windows that can be closed)
			bool showDemoWindow = false;
			bool showSettingsWindow = false;
			bool showStyleSelect = false;

			// Grid stuff
			bool snapToGrid = false;
			bool drawGrid = false;
			glm::vec2 gridSize = { 32.0f, 32.0f };
			glm::vec3 gridColor = { 0.1f, 0.1f, 0.1f };
			float gridStrokeWidth = 1.0f;

			// Selected Style
			int selectedStyle = 0;
		}

		namespace EditorStyle
		{
			// Fonts
			ImFont* defaultFont = nullptr;
			ImFont* largeIconFont = nullptr;

			// Style Colors
			glm::vec4 mainBgLight0{ 0.404f, 0.404f ,0.404f, 1.0f };
			glm::vec4 mainBg{ 0.21f, 0.21f, 0.21f, 1.0f };
			glm::vec4 mainBgDark0{ 0.190f, 0.190f, 0.190f, 1.0f };
			glm::vec4 mainBgDark1{ 0.145f, 0.145f, 0.145f, 1.0f };
			glm::vec4 mainBgDark2{ 0.098f, 0.098f, 0.098f, 1.0f };

			glm::vec4 accent{ 0.149f, 0.149f, 0.149f, 1.0f };
			glm::vec4 accentDark0{ 0.102f, 0.102f, 0.102f, 1.0f };
			glm::vec4 accentDark1{ 0.063f, 0.063f, 0.063f, 1.0f };

			glm::vec4 button{ 0.882f, 0.882f, 0.882f, 1.0f };
			glm::vec4 buttonHovered{ 0.782f, 0.782f, 0.782f, 1.0f };
			
			glm::vec4 header{ 0.338f, 0.338f, 0.338f, 1.0f };
			glm::vec4 headerHovered{ 0.276f, 0.276f, 0.276f, 1.0f };
			glm::vec4 headerActive{ 0.379f, 0.379f, 0.379f, 1.0f };

			glm::vec4 font{ 0.902f, 0.902f, 0.902f, 1.0f };
			glm::vec4 fontDisabled{ 0.36f, 0.36f, 0.36f, 1.0f };
			glm::vec4 highlightColor{ 0.145f, 0.553f, 0.384f, 1.0f };

			// Sizing
			glm::vec2 windowPadding{ 10, 10 };
			glm::vec2 framePadding{ 20, 8 };
			glm::vec2 itemSpacing{ 20, 8 };
			float scrollbarSize = 17;
			float scrollbarRounding = 12;
			float frameRounding = 8;
			float grabRounding = 8;
			float tabRounding = 8;
		}
	}
}