#ifndef COCOA_ENGINE_KEY_EVENT_H
#define COCOA_ENGINE_KEY_EVENT_H

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
		int getKeyCode() const;

		EVENT_CLASS_CATEGORY_HEADER(EventCategoryKeyboard | EventCategoryInput)

	protected:
		// Abstract class, should not be instantiated
		KeyEvent(int keycode);

		int mKeyCode;
	};

	// ============================================================
	// KeyPressed
	// ============================================================
	class COCOA KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(int keycode, int repeatCount);

		inline int getRepeatCount() const;

		std::string toString() const override;

		EVENT_CLASS_TYPE_HEADER(KeyPressed)

	private:
		int mRepeatCount;
	};

	// ============================================================
	// KeyReleased
	// ============================================================
	class COCOA KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(int keycode);

		std::string toString() const override;

		EVENT_CLASS_TYPE_HEADER(KeyReleased);
	};

	// ============================================================
	// KeyTyped
	// ============================================================
	class COCOA KeyTypedEvent : public KeyEvent
	{
		KeyTypedEvent(int keycode);

		std::string toString() const override;

		EVENT_CLASS_TYPE_HEADER(KeyTyped);
	};
}

#endif
