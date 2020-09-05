#include "Util/Settings.h"

namespace Jade
{
	namespace Settings
	{
        // =======================================================================
        // Editor Style Settings
        // =======================================================================

        // Fonts
        ImFont* EditorStyle::s_DefaultFont = nullptr;
        ImFont* EditorStyle::s_LargeIconFont = nullptr;
        
        // Style Colors
        glm::vec4 EditorStyle::s_MainBgLight0{ 0.404f, 0.404f ,0.404f, 1.0f };
        glm::vec4 EditorStyle::s_MainBg{ 0.212f, 0.212f, 0.212f, 1.0f };
        glm::vec4 EditorStyle::s_MainBgDark0{ 0.190f, 0.190f, 0.190f, 1.0f };
        glm::vec4 EditorStyle::s_MainBgDark1{ 0.145f, 0.145f, 0.145f, 1.0f };
        glm::vec4 EditorStyle::s_MainBgDark2{ 0.098f, 0.098f, 0.098f, 1.0f };

        glm::vec4 EditorStyle::s_Accent{ 0.149f, 0.149f, 0.149f, 1.0f };
        glm::vec4 EditorStyle::s_AccentDark0{ 0.102f, 0.102f, 0.102f, 1.0f };
        glm::vec4 EditorStyle::s_AccentDark1{ 0.063f, 0.063f, 0.063f, 1.0f };

        glm::vec4 EditorStyle::s_Button{ 0.882f, 0.882f, 0.882f, 1.0f };
        glm::vec4 EditorStyle::s_ButtonHovered{ 0.782f, 0.782f, 0.782f, 1.0f };

        glm::vec4 EditorStyle::s_Font{ 0.902f, 0.902f, 0.902f, 1.0f };
        glm::vec4 EditorStyle::s_FontDisabled{ 0.839f, 0.839f, 0.839f, 1.0f };
        glm::vec4 EditorStyle::s_HighlightColor{ 0.145f, 0.553f, 0.384f, 1.0f };

        // Sizing
        glm::vec2 EditorStyle::s_WindowPadding{ 10, 10 };
        glm::vec2 EditorStyle::s_FramePadding{ 20, 8 };
        glm::vec2 EditorStyle::s_ItemSpacing{ 20, 8 };
        float EditorStyle::s_ScrollbarSize = 17;
        float EditorStyle::s_ScrollbarRounding = 12;
        float EditorStyle::s_FrameRounding = 8;
        float EditorStyle::s_GrabRounding = 8;
        float EditorStyle::s_TabRounding = 8;
	}
}