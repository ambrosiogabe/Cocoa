#include "externalLibs.h"

#include "cocoa/events/WindowEvent.h"

namespace Cocoa
{
	WindowResizeEvent::WindowResizeEvent(uint32 width, uint32 height)
		: m_Width(width), m_Height(height)
	{
	}

	uint32 WindowResizeEvent::getWidth() const
	{
		return m_Width;
	}

	uint32 WindowResizeEvent::getHeight() const
	{
		return m_Height;
	}

	std::string WindowResizeEvent::toString() const
	{
		std::stringstream ss;
		ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
		return ss.str();
	}

	EVENT_CLASS_TYPE_IMPL(WindowResize, WindowResizeEvent)
		EVENT_CLASS_CATEGORY_IMPL(EventCategoryApplication, WindowResizeEvent)

		WindowCloseEvent::WindowCloseEvent()
	{

	}

	EVENT_CLASS_TYPE_IMPL(WindowClose, WindowCloseEvent)
		EVENT_CLASS_CATEGORY_IMPL(EventCategoryApplication, WindowCloseEvent)
}