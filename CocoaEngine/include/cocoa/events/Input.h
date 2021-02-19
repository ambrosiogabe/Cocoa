#pragma once
#include "externalLibs.h"
#include "cocoa/scenes/SceneData.h"
#include "cocoa/renderer/Camera.h"

namespace Cocoa
{
	class COCOA Input
	{
	public:
		static void Init();
		static void SetScene(SceneData* scene);

		static void KeyCallback(int key, int scancode, int action, int mods);
		static void CursorCallback(double xpos, double ypos);
		static void MouseButtonCallback(int button, int action, int mods);
		static void ScrollCallback(double xoffset, double yoffset);

		static bool KeyPressed(int keyCode);
		static bool MouseButtonPressed(int mouseButton);
		static float MouseX();
		static float MouseY();
		static float ScrollX();
		static float ScrollY();
		static void EndFrame();
		static glm::vec2 MousePos();

		static float OrthoMouseX();
		static float OrthoMouseY();
		static glm::vec2 ScreenToOrtho(const Camera& camera);
		static glm::vec2 NormalizedMousePos();

		static void SetGameViewPos(const glm::vec2& position);
		static void SetGameViewSize(const glm::vec2& size);
		static void SetGameViewMousePos(const glm::vec2& mousePos);

	private:
		Input::Input() {}

	private:
		static bool s_Initialized;

		static bool s_KeyPressed[349];
		static bool s_MouseButtonPressed[3];
		static float s_XPos;
		static float s_YPos;
		static float s_ScrollX;
		static float s_ScrollY;

		static glm::vec2 s_GameViewPos;
		static glm::vec2 s_GameViewSize;
		static glm::vec2 s_GameViewMousePos;

		static SceneData* s_Scene;
	};
}