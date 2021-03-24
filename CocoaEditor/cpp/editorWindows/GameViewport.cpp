#include "editorWindows/GameViewport.h"
#include "util/Settings.h"

#include "cocoa/util/Settings.h"
#include "cocoa/scenes/Scene.h"
#include "cocoa/core/Application.h"
#include "cocoa/systems/RenderSystem.h"

#include <imgui.h>

namespace Cocoa
{
	namespace GameViewport
	{
		// Internal variables
		static glm::vec2 m_GameviewPos = glm::vec2();
		static glm::vec2 m_GameviewSize = glm::vec2();
		static glm::vec2 m_GameviewMousePos = glm::vec2();
		static bool m_BlockEvents = false;

		void ImGui(SceneData& scene)
		{
			ImGui::Begin("Game Viewport");

			ImGui::End();
		}
	}
}