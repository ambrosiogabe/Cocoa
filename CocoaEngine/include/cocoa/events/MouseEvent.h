#ifndef COCOA_ENGINE_MOUSE_EVENT_H
#define COCOA_ENGINE_MOUSE_EVENT_H

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

		inline float getX() const;
		inline float getY() const;

		std::string toString() const override;

		EVENT_CLASS_TYPE_HEADER(MouseMoved)
			EVENT_CLASS_CATEGORY_HEADER(EventCategoryInput | EventCategoryMouse)

	private:
		float mMouseX;
		float mMouseY;
	};

	// ===============================================================
	// MouseScrolledEvent
	// ===============================================================
	class COCOA MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(float xOffset, float yOffset);

		// TODO: Make these inline somehow
		float getXOffset() const;
		float getYOffset() const;

		std::string toString() const override;

		EVENT_CLASS_TYPE_HEADER(MouseScrolled)
			EVENT_CLASS_CATEGORY_HEADER(EventCategoryMouse | EventCategoryInput)

	private:
		float mXOffset;
		float mYOffset;
	};

	// ===============================================================
	// Base
	// ===============================================================
	class COCOA MouseButtonEvent : public Event
	{
	public:
		// TODO: Make this inline somehow
		int getMouseButton() const;

		EVENT_CLASS_CATEGORY_HEADER(EventCategoryInput | EventCategoryMouse)

	protected:
		MouseButtonEvent(int button);

		int mButton;
	};

	// ===============================================================
	// MouseButtonPressedEvent
	// ===============================================================
	class COCOA MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(int button);

		std::string toString() const override;

		EVENT_CLASS_TYPE_HEADER(MouseButtonPressed)
	};

	// ===============================================================
	// MouseButtonReleasedEvent
	// ===============================================================
	class COCOA MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(int button);

		std::string toString() const override;

		EVENT_CLASS_TYPE_HEADER(MouseButtonReleased)
	};
}

#endif
