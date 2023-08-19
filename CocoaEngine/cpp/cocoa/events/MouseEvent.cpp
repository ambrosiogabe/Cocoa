#include "externalLibs.h"

#include "cocoa/events/MouseEvent.h"

namespace Cocoa
{
	// ===============================================================
	// MouseMovedEvent
	// ===============================================================
	MouseMovedEvent::MouseMovedEvent(float x, float y)
		: mMouseX(x), mMouseY(y)
	{
	}

	float MouseMovedEvent::getX() const
	{
		return mMouseX;
	}

	float MouseMovedEvent::getY() const
	{
		return mMouseY;
	}

	std::string MouseMovedEvent::toString() const
	{
		std::stringstream ss;
		ss << "MouseMovedEvent: " << mMouseX << ", " << mMouseY;
		return ss.str();
	}

	EVENT_CLASS_TYPE_IMPL(MouseMoved, MouseMovedEvent)
		EVENT_CLASS_CATEGORY_IMPL(EventCategoryMouse | EventCategoryInput, MouseMovedEvent)

		// ===============================================================
		// MouseScrolledEvent
		// ===============================================================
		MouseScrolledEvent::MouseScrolledEvent(float xOffset, float yOffset)
		: mXOffset(xOffset), mYOffset(yOffset)
	{
	}

	float MouseScrolledEvent::getXOffset() const
	{
		return mXOffset;
	}

	float MouseScrolledEvent::getYOffset() const
	{
		return mYOffset;
	}

	std::string MouseScrolledEvent::toString() const
	{
		std::stringstream ss;
		ss << "MouseScrolledEvent: " << mXOffset << ", " << mYOffset;
		return ss.str();
	}

	EVENT_CLASS_TYPE_IMPL(MouseScrolled, MouseScrolledEvent)
		EVENT_CLASS_CATEGORY_IMPL(EventCategoryMouse | EventCategoryInput, MouseScrolledEvent)

		// ===============================================================
		// Base
		// ===============================================================
		MouseButtonEvent::MouseButtonEvent(int button)
		: mButton(button)
	{
	}

	int MouseButtonEvent::getMouseButton() const
	{
		return mButton;
	}

	EVENT_CLASS_CATEGORY_IMPL(EventCategoryMouse | EventCategoryInput, MouseButtonEvent)

		// ===============================================================
		// MouseButtonPressedEvent
		// ===============================================================
		MouseButtonPressedEvent::MouseButtonPressedEvent(int button)
		: MouseButtonEvent(button)
	{
	}

	std::string MouseButtonPressedEvent::toString() const
	{
		std::stringstream ss;
		ss << "MouseButtonPressedEvent: " << mButton;
		return ss.str();
	}

	EVENT_CLASS_TYPE_IMPL(MouseButtonPressed, MouseButtonPressedEvent)

		// ===============================================================
		// MouseButtonReleasedEvent
		// ===============================================================
		MouseButtonReleasedEvent::MouseButtonReleasedEvent(int button)
		: MouseButtonEvent(button)
	{
	}

	std::string MouseButtonReleasedEvent::toString() const
	{
		std::stringstream ss;
		ss << "MouseButtonPressedEvent: " << mButton;
		return ss.str();
	}

	EVENT_CLASS_TYPE_IMPL(MouseButtonReleased, MouseButtonReleasedEvent)
}