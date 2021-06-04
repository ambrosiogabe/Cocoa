#include "editorWindows/GameViewport.h"
#include "util/Settings.h"
#include "core/LevelEditorSystem.h"

#include "cocoa/util/Settings.h"
#include "cocoa/scenes/Scene.h"
#include "cocoa/core/Application.h"
#include "cocoa/systems/RenderSystem.h"
#include "cocoa/components/NoSerialize.h"

#include <imgui.h>

namespace Cocoa
{
	namespace GameViewport
	{
		// Internal variables
		static bool m_BlockEvents = false;

		void ImGui(SceneData& scene)
		{
			static bool open = true;
			ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 1));
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			ImGui::Begin("Game Viewport", &open, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_MenuBar);

			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 1);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 1);

			ImVec2 windowSize = ImGui::GetContentRegionAvail() - ImVec2(1, 1);

			// Figure out the largest area that fits this target aspect ratio
			float aspectWidth = windowSize.x;
			float aspectHeight = (float)aspectWidth / Application::Get()->GetWindow()->GetTargetAspectRatio();
			if (aspectHeight > windowSize.y)
			{
				// It doesn't fit our height, we must switch to pillarbox
				aspectHeight = windowSize.y;
				aspectWidth = (float)aspectHeight * Application::Get()->GetWindow()->GetTargetAspectRatio();
			}

			// Center rectangle
			float vpX = (windowSize.x / 2.0f) - ((float)aspectWidth / 2.0f);
			float vpY = ((float)windowSize.y / 2.0f) - ((float)aspectHeight / 2.0f);

			ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(vpX, vpY));

			auto view = scene.Registry.view<Camera>(entt::exclude<NoSerialize>);
			int i = 0;
			for (auto& rawEntity : view)
			{
				Logger::Assert(i == 0, "Game viewport does not support multiple cameras yet.");
				const Camera& camera = scene.Registry.get<Camera>(rawEntity);
				uint32 texId = NFramebuffer::GetColorAttachment(camera.Framebuffer, 0).GraphicsId;
				ImGui::Image(reinterpret_cast<void*>(texId), ImVec2(aspectWidth, aspectHeight), ImVec2(0, 1), ImVec2(1, 0));
				i++;
			}

			ImGui::End();
			ImGui::PopStyleColor();
			ImGui::PopStyleVar();
		}
	}
}