#include "externalLibs.h"
#include "cocoa/core/Core.h"

#include "cocoa/events/Input.h"
#include "cocoa/core/Application.h"
#include "cocoa/renderer/Camera.h"
#include "cocoa/scenes/Scene.h"
#include "cocoa/scenes/SceneData.h"

namespace Cocoa
{
	bool Input::s_Initialized = false;
	bool Input::s_KeyPressed[349]{ };
	bool Input::s_MouseButtonPressed[3]{ };
	float Input::s_XPos = 0.0f;
	float Input::s_YPos = 0.0f;
	float Input::s_ScrollX = 0.0f;
	float Input::s_ScrollY = 0.0f;
	glm::vec2 Input::s_GameViewPos{ 0, 0 };
	glm::vec2 Input::s_GameViewSize{ 0, 0 };
	glm::vec2 Input::s_GameViewMousePos{ 0, 0 };

	void Input::Init()
	{
		Logger::Assert(!s_Initialized, "Input already initialized.");
		s_Initialized = true;
		const glm::vec2& windowSize = Application::Get()->GetWindow()->GetSize();
	}

	void Input::KeyCallback(int key, int scancode, int action, int mods)
	{
		s_KeyPressed[key] = (action == COCOA_PRESS || action == COCOA_REPEAT) ? true : false;
	}

	bool Input::KeyPressed(int keyCode)
	{
		if (keyCode >= 0 && keyCode < 350)
		{
			return s_KeyPressed[keyCode];
		}
		return false;
	}

	void Input::MouseButtonCallback(int button, int action, int mods)
	{
		s_MouseButtonPressed[button] = action == COCOA_PRESS ? true : false;
	}

	bool Input::MouseButtonPressed(int mouseButton)
	{
		if (mouseButton >= 0 && mouseButton < 3)
		{
			return s_MouseButtonPressed[mouseButton];
		}
		return false;
	}

	void Input::CursorCallback(double xpos, double ypos)
	{
		s_XPos = (float)xpos;
		s_YPos = (float)ypos;
	}

	float Input::MouseX()
	{
		return s_XPos;
	}

	float Input::MouseY()
	{
		return s_YPos;
	}

	float Input::OrthoMouseX(const Camera& camera)
	{
		float currentX = MouseX() - s_GameViewPos.x;
		currentX = (currentX / s_GameViewSize.x) * 2.0f - 1.0f;
		glm::vec4 tmp = glm::vec4(currentX, 0.0f, 0.0f, 1.0f);
		tmp = camera.InverseView * camera.InverseProjection * tmp;

		return tmp.x;
	}

	float Input::OrthoMouseY(const Camera& camera)
	{
		float currentY = s_GameViewPos.y - MouseY();
		currentY = (currentY / s_GameViewSize.y) * 2.0f - 1.0f;
		glm::vec4 tmp = glm::vec4(0.0f, currentY, 0.0f, 1.0f);
		tmp = camera.InverseView * camera.InverseProjection * tmp;

		return tmp.y;
	}

	glm::vec2 Input::ScreenToOrtho(const Camera& camera)
	{
		glm::vec4 tmp{ s_GameViewMousePos.x, s_GameViewMousePos.y, 0, 1 };

		tmp.x = (tmp.x / s_GameViewSize.x) * 2.0f - 1.0f;
		tmp.y = -((tmp.y / s_GameViewSize.y) * 2.0f - 1.0f);
		tmp = camera.InverseView * camera.InverseProjection * tmp;

		return glm::vec2{ tmp.x, tmp.y };
	}

	glm::vec2 Input::NormalizedMousePos()
	{
		glm::vec4 tmp{ s_GameViewMousePos.x, s_GameViewSize.y - s_GameViewMousePos.y, 0, 1 };

		tmp.x = (tmp.x / s_GameViewSize.x);
		tmp.y = (tmp.y / s_GameViewSize.y);

		return glm::vec2{ tmp.x, tmp.y };
	}

	void Input::ScrollCallback(double xpos, double ypos)
	{
		s_ScrollX = (float)xpos;
		s_ScrollY = -(float)ypos;
	}

	float Input::ScrollX()
	{
		return s_ScrollX;
	}

	float Input::ScrollY()
	{
		return s_ScrollY;
	}

	void Input::EndFrame()
	{
		s_ScrollX = 0;
		s_ScrollY = 0;
	}

	glm::vec2 Input::MousePos()
	{
		return glm::vec2{ s_XPos, s_YPos };
	}

	void Input::SetGameViewPos(const glm::vec2& position)
	{
		s_GameViewPos.x = position.x;
		s_GameViewPos.y = position.y;
	}

	void Input::SetGameViewSize(const glm::vec2& size)
	{
		s_GameViewSize.x = size.x;
		s_GameViewSize.y = size.y;
	}

	void Input::SetGameViewMousePos(const glm::vec2& mousePos)
	{
		s_GameViewMousePos.x = mousePos.x;
		s_GameViewMousePos.y = mousePos.y;
	}
}