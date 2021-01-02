#pragma once

#include "cocoa/events/Event.h"

namespace Cocoa
{
	// ============================================================
	// Base
	// ============================================================
	class COCOA KeyEvent : public Event
	{
	public:
		// TODO: Make this inline somehow
		int GetKeyCode() const;

		EVENT_CLASS_CATEGORY_HEADER(EventCategoryKeyboard | EventCategoryInput)

	protected:
		// Abstract class, should not be instantiated
		KeyEvent(int keycode);

		int m_KeyCode;
	};

	// ============================================================
	// KeyPressed
	// ============================================================
	class COCOA KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(int keycode, int repeatCount);

		inline int GetRepeatCount() const;

		virtual std::string ToString() const override;

		EVENT_CLASS_TYPE_HEADER(KeyPressed)

	private:
		int m_RepeatCount;
	};

	// ============================================================
	// KeyReleased
	// ============================================================
	class COCOA KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(int keycode);

		virtual std::string ToString() const override;

		EVENT_CLASS_TYPE_HEADER(KeyReleased);
	};

	// ============================================================
	// KeyTyped
	// ============================================================
	class COCOA KeyTypedEvent : public KeyEvent
	{
		KeyTypedEvent(int keycode);

		virtual std::string ToString() const override;

		EVENT_CLASS_TYPE_HEADER(KeyTyped);
	};
}