#include "externalLibs.h"

#include "cocoa/events/MouseEvent.h"

namespace Cocoa
{
	// ===============================================================
	// MouseMovedEvent
	// ===============================================================
	MouseMovedEvent::MouseMovedEvent(float x, float y)
		: m_MouseX(x), m_MouseY(y)
	{
	}

	float MouseMovedEvent::GetX() const
	{
		return m_MouseX;
	}

	float MouseMovedEvent::GetY() const
	{
		return m_MouseY;
	}

	std::string MouseMovedEvent::ToString() const
	{
		std::stringstream ss;
		ss << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY;
		return ss.str();
	}

	EVENT_CLASS_TYPE_IMPL(MouseMoved, MouseMovedEvent)
		EVENT_CLASS_CATEGORY_IMPL(EventCategoryMouse | EventCategoryInput, MouseMovedEvent)

		// ===============================================================
		// MouseScrolledEvent
		// ===============================================================
		MouseScrolledEvent::MouseScrolledEvent(float xoffset, float yoffset)
		: m_XOffset(xoffset), m_YOffset(yoffset)
	{
	}

	float MouseScrolledEvent::GetXOffset() const
	{
		return m_XOffset;
	}

	float MouseScrolledEvent::GetYOffset() const
	{
		return m_YOffset;
	}

	std::string MouseScrolledEvent::ToString() const
	{
		std::stringstream ss;
		ss << "MouseScrolledEvent: " << m_XOffset << ", " << m_YOffset;
		return ss.str();
	}

	EVENT_CLASS_TYPE_IMPL(MouseScrolled, MouseScrolledEvent)
		EVENT_CLASS_CATEGORY_IMPL(EventCategoryMouse | EventCategoryInput, MouseScrolledEvent)

		// ===============================================================
		// Base
		// ===============================================================
		MouseButtonEvent::MouseButtonEvent(int button)
		: m_Button(button)
	{
	}

	int MouseButtonEvent::GetMouseButton() const
	{
		return m_Button;
	}

	EVENT_CLASS_CATEGORY_IMPL(EventCategoryMouse | EventCategoryInput, MouseButtonEvent)

		// ===============================================================
		// MouseButtonPressedEvent
		// ===============================================================
		MouseButtonPressedEvent::MouseButtonPressedEvent(int button)
		: MouseButtonEvent(button)
	{
	}

	std::string MouseButtonPressedEvent::ToString() const
	{
		std::stringstream ss;
		ss << "MouseButtonPressedEvent: " << m_Button;
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

	std::string MouseButtonReleasedEvent::ToString() const
	{
		std::stringstream ss;
		ss << "MouseButtonPressedEvent: " << m_Button;
		return ss.str();
	}

	EVENT_CLASS_TYPE_IMPL(MouseButtonReleased, MouseButtonReleasedEvent)
}