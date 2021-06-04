#include "externalLibs.h"
#include "cocoa/core/Core.h"

#include "cocoa/events/Input.h"
#include "cocoa/core/Application.h"
#include "cocoa/renderer/Camera.h"
#include "cocoa/scenes/Scene.h"
#include "cocoa/scenes/SceneData.h"

namespace Cocoa
{
	bool Input::sInitialized = false;
	bool Input::sKeyPressed[349]{ };
	bool Input::sMouseButtonPressed[3]{ };
	float Input::sXPos = 0.0f;
	float Input::sYPos = 0.0f;
	float Input::sScrollX = 0.0f;
	float Input::sScrollY = 0.0f;
	glm::vec2 Input::sGameViewPos{ 0, 0 };
	glm::vec2 Input::sGameViewSize{ 0, 0 };
	glm::vec2 Input::sGameViewMousePos{ 0, 0 };

	void Input::init()
	{
		Logger::Assert(!sInitialized, "Input already initialized.");
		sInitialized = true;
		const glm::vec2& windowSize = Application::get()->getWindow()->getSize();
	}

	void Input::keyCallback(int key, int scanCode, int action, int mods)
	{
		sKeyPressed[key] = (action == COCOA_PRESS || action == COCOA_REPEAT) ? true : false;
	}

	bool Input::keyPressed(int keyCode)
	{
		if (keyCode >= 0 && keyCode < 350)
		{
			return sKeyPressed[keyCode];
		}
		return false;
	}

	void Input::mouseButtonCallback(int button, int action, int mods)
	{
		sMouseButtonPressed[button] = action == COCOA_PRESS ? true : false;
	}

	bool Input::mouseButtonPressed(int mouseButton)
	{
		if (mouseButton >= 0 && mouseButton < 3)
		{
			return sMouseButtonPressed[mouseButton];
		}
		return false;
	}

	void Input::cursorCallback(double xPos, double yPos)
	{
		sXPos = (float)xPos;
		sYPos = (float)yPos;
	}

	float Input::mouseX()
	{
		return sXPos;
	}

	float Input::mouseY()
	{
		return sYPos;
	}

	float Input::orthoMouseX(const Camera& camera)
	{
		float currentX = mouseX() - sGameViewPos.x;
		currentX = (currentX / sGameViewSize.x) * 2.0f - 1.0f;
		glm::vec4 tmp = glm::vec4(currentX, 0.0f, 0.0f, 1.0f);
		tmp = camera.InverseView * camera.InverseProjection * tmp;

		return tmp.x;
	}

	float Input::orthoMouseY(const Camera& camera)
	{
		float currentY = sGameViewPos.y - mouseY();
		currentY = (currentY / sGameViewSize.y) * 2.0f - 1.0f;
		glm::vec4 tmp = glm::vec4(0.0f, currentY, 0.0f, 1.0f);
		tmp = camera.InverseView * camera.InverseProjection * tmp;

		return tmp.y;
	}

	glm::vec2 Input::screenToOrtho(const Camera& camera)
	{
		glm::vec4 tmp{ sGameViewMousePos.x, sGameViewMousePos.y, 0, 1 };

		tmp.x = (tmp.x / sGameViewSize.x) * 2.0f - 1.0f;
		tmp.y = -((tmp.y / sGameViewSize.y) * 2.0f - 1.0f);
		tmp = camera.InverseView * camera.InverseProjection * tmp;

		return glm::vec2{ tmp.x, tmp.y };
	}

	glm::vec2 Input::normalizedMousePos()
	{
		glm::vec4 tmp{ sGameViewMousePos.x, sGameViewSize.y - sGameViewMousePos.y, 0, 1 };

		tmp.x = (tmp.x / sGameViewSize.x);
		tmp.y = (tmp.y / sGameViewSize.y);

		return glm::vec2{ tmp.x, tmp.y };
	}

	void Input::scrollCallback(double xOffset, double yOffset)
	{
		sScrollX = (float)xOffset;
		sScrollY = -(float)yOffset;
	}

	float Input::scrollX()
	{
		return sScrollX;
	}

	float Input::scrollY()
	{
		return sScrollY;
	}

	void Input::endFrame()
	{
		sScrollX = 0;
		sScrollY = 0;
	}

	glm::vec2 Input::mousePos()
	{
		return glm::vec2{ sXPos, sYPos };
	}

	void Input::setGameViewPos(const glm::vec2& position)
	{
		sGameViewPos.x = position.x;
		sGameViewPos.y = position.y;
	}

	void Input::setGameViewSize(const glm::vec2& size)
	{
		sGameViewSize.x = size.x;
		sGameViewSize.y = size.y;
	}

	void Input::setGameViewMousePos(const glm::vec2& mousePos)
	{
		sGameViewMousePos.x = mousePos.x;
		sGameViewMousePos.y = mousePos.y;
	}
}