#pragma once

#include "cocoa/events/Event.h"

namespace Cocoa
{
	// ===============================================================
	// MouseMovedEvent
	// ===============================================================
	class COCOA MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(float x, float y);

		inline float GetX() const;
		inline float GetY() const;

		std::string ToString() const override;

		EVENT_CLASS_TYPE_HEADER(MouseMoved)
			EVENT_CLASS_CATEGORY_HEADER(EventCategoryInput | EventCategoryMouse)

	private:
		float m_MouseX;
		float m_MouseY;
	};

	// ===============================================================
	// MouseScrolledEvent
	// ===============================================================
	class COCOA MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(float xoffset, float yoffset);

		// TODO: Make these inline somehow
		float GetXOffset() const;
		float GetYOffset() const;

		std::string ToString() const override;

		EVENT_CLASS_TYPE_HEADER(MouseScrolled)
			EVENT_CLASS_CATEGORY_HEADER(EventCategoryMouse | EventCategoryInput)

	private:
		float m_XOffset;
		float m_YOffset;
	};

	// ===============================================================
	// Base
	// ===============================================================
	class COCOA MouseButtonEvent : public Event
	{
	public:
		// TODO: Make this inline somehow
		int GetMouseButton() const;

		EVENT_CLASS_CATEGORY_HEADER(EventCategoryInput | EventCategoryMouse)

	protected:
		MouseButtonEvent(int button);

		int m_Button;
	};

	// ===============================================================
	// MouseButtonPressedEvent
	// ===============================================================
	class COCOA MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(int button);

		std::string ToString() const override;

		EVENT_CLASS_TYPE_HEADER(MouseButtonPressed)
	};

	// ===============================================================
	// MouseButtonReleasedEvent
	// ===============================================================
	class COCOA MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(int button);

		std::string ToString() const override;

		EVENT_CLASS_TYPE_HEADER(MouseButtonReleased)
	};
}