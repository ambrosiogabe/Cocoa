#ifndef COCOA_ENGINE_INPUT_H
#define COCOA_ENGINE_INPUT_H
#include "externalLibs.h"
#include "cocoa/scenes/SceneData.h"
#include "cocoa/renderer/Camera.h"

namespace Cocoa
{
	class COCOA Input
	{
	public:
		static void init();

		static void keyCallback(int key, int scanCode, int action, int mods);
		static void cursorCallback(double xPos, double yPos);
		static void mouseButtonCallback(int button, int action, int mods);
		static void scrollCallback(double xOffset, double yOffset);

		static bool keyPressed(int keyCode);
		static bool mouseButtonPressed(int mouseButton);
		static float mouseX();
		static float mouseY();
		static float scrollX();
		static float scrollY();
		static void endFrame();
		static glm::vec2 mousePos();

		static float orthoMouseX(const Camera& camera);
		static float orthoMouseY(const Camera& camera);
		static glm::vec2 screenToOrtho(const Camera& camera);
		static glm::vec2 normalizedMousePos();

		static void setGameViewPos(const glm::vec2& position);
		static void setGameViewSize(const glm::vec2& size);
		static void setGameViewMousePos(const glm::vec2& mousePos);

	private:
		Input() = default;

	private:
		static bool sInitialized;

		static bool sKeyPressed[349];
		static bool sMouseButtonPressed[3];
		static float sXPos;
		static float sYPos;
		static float sScrollX;
		static float sScrollY;

		static glm::vec2 sGameViewPos;
		static glm::vec2 sGameViewSize;
		static glm::vec2 sGameViewMousePos;

		static SceneData* sScene;
	};
}

#endif
