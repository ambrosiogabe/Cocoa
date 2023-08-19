#include "externalLibs.h"

#include "cocoa/events/KeyEvent.h"

namespace Cocoa
{
	// ============================================================
	// Base
	// ============================================================
	KeyEvent::KeyEvent(int keycode)
		: mKeyCode(keycode)
	{
	}

	int KeyEvent::getKeyCode() const
	{
		return mKeyCode;
	}

	EVENT_CLASS_CATEGORY_IMPL(EventCategoryKeyboard | EventCategoryInput, KeyEvent)

		// ============================================================
		// KeyPressed
		// ============================================================
		KeyPressedEvent::KeyPressedEvent(int keycode, int repeatCount)
		: KeyEvent(keycode), mRepeatCount(repeatCount)
	{
	}

	int KeyPressedEvent::getRepeatCount() const
	{
		return mRepeatCount;
	}

	std::string KeyPressedEvent::toString() const
	{
		std::stringstream ss;
		ss << "KeyPressedEvent: " << mKeyCode << "(" << mRepeatCount << ")";
		return ss.str();
	}

	EVENT_CLASS_TYPE_IMPL(KeyPressed, KeyPressedEvent)

		// ============================================================
		// KeyReleased
		// ============================================================
		KeyReleasedEvent::KeyReleasedEvent(int keycode)
		: KeyEvent(keycode)
	{
	}

	std::string KeyReleasedEvent::toString() const
	{
		std::stringstream ss;
		ss << "KeyReleasedEvent: " << mKeyCode;
		return ss.str();
	}

	EVENT_CLASS_TYPE_IMPL(KeyReleased, KeyReleasedEvent)

		// ============================================================
		// KeyTyped
		// ============================================================
		KeyTypedEvent::KeyTypedEvent(int keycode)
		: KeyEvent(keycode)
	{
	}

	std::string KeyTypedEvent::toString() const
	{
		std::stringstream ss;
		ss << "KeyTypedEvent: " << mKeyCode;
		return ss.str();
	}

	EVENT_CLASS_TYPE_IMPL(KeyTyped, KeyTypedEvent)
}