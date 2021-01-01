#include "externalLibs.h"

#include "cocoa/events/KeyEvent.h"

namespace Cocoa
{
	// ============================================================
	// Base
	// ============================================================
	KeyEvent::KeyEvent(int keycode)
		: m_KeyCode(keycode)
	{
	}

	int KeyEvent::GetKeyCode() const
	{
		return m_KeyCode;
	}

	EVENT_CLASS_CATEGORY_IMPL(EventCategoryKeyboard | EventCategoryInput, KeyEvent)

		// ============================================================
		// KeyPressed
		// ============================================================
		KeyPressedEvent::KeyPressedEvent(int keycode, int repeatCount)
		: KeyEvent(keycode), m_RepeatCount(repeatCount)
	{
	}

	int KeyPressedEvent::GetRepeatCount() const
	{
		return m_RepeatCount;
	}

	std::string KeyPressedEvent::ToString() const
	{
		std::stringstream ss;
		ss << "KeyPressedEvent: " << m_KeyCode << "(" << m_RepeatCount << ")";
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

	std::string KeyReleasedEvent::ToString() const
	{
		std::stringstream ss;
		ss << "KeyReleasedEvent: " << m_KeyCode;
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

	std::string KeyTypedEvent::ToString() const
	{
		std::stringstream ss;
		ss << "KeyTypedEvent: " << m_KeyCode;
		return ss.str();
	}

	EVENT_CLASS_TYPE_IMPL(KeyTyped, KeyTypedEvent)
}