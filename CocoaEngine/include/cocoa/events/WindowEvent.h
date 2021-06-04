#ifndef COCOA_ENGINE_WINDOW_EVENT_H
#define COCOA_ENGINE_WINDOW_EVENT_H

#include "cocoa/core/Core.h"
#include "cocoa/events/Event.h"

namespace Cocoa
{
	class COCOA WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(uint32 width, uint32 height);

		// TODO: Make these inline somehow
		uint32 getWidth() const;
		uint32 getHeight() const;

		std::string toString() const override;

		EVENT_CLASS_TYPE_HEADER(WindowResize)
			EVENT_CLASS_CATEGORY_HEADER(EventCategoryApplication)

	private:
		uint32 m_Width;
		uint32 m_Height;
	};

	class COCOA WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent();

		EVENT_CLASS_TYPE_HEADER(WindowClose)
			EVENT_CLASS_CATEGORY_HEADER(EventCategoryApplication)
	};
}

#endif
