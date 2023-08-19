#ifndef COCOA_ENGINE_EVENT_H
#define COCOA_ENGINE_EVENT_H
#include "externalLibs.h"
#include "cocoa/core/Core.h"

#pragma warning( push )
#pragma warning ( disable : 26812 )

namespace Cocoa
{

	// ================================================================
	// Event enums
	// We have to declare the first enum as a class, otherwise we get
	// a name clashing error for None = 0
	// ================================================================
	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum EventCategory
	{
		None = 0,
		EventCategoryApplication = 0x00000001,
		EventCategoryInput = 0x00000010,
		EventCategoryKeyboard = 0x00000100,
		EventCategoryMouse = 0x00001000,
		EventCategoryMouseButton = 0x00010000
	};

	// ================================================================
	// Event helper macros
	// ================================================================
	#define EVENT_CLASS_TYPE_HEADER(type) static EventType getStaticType(); \
											virtual EventType getType() const override; \
											virtual const char* getName() const override;

	#define EVENT_CLASS_TYPE_IMPL(type, clazz) EventType clazz::getStaticType() { return EventType::##type; } \
										EventType clazz::getType() const { return getStaticType(); } \
										const char* clazz::getName() const { return #type; }

	#define EVENT_CLASS_CATEGORY_HEADER(category) int getCategoryFlags() const;
	#define EVENT_CLASS_CATEGORY_IMPL(category, clazz) int clazz::getCategoryFlags() const { return category; }

	// ================================================================
	// Event Base Class
	// ================================================================
	class COCOA Event
	{
		friend class EventDispatcher;

	public:
		virtual EventType getType() const = 0;
		virtual const char* getName() const = 0;
		virtual int getCategoryFlags() const = 0;
		virtual std::string toString() const { return getName(); }

		bool isInCategory(EventCategory category) const { return getCategoryFlags() & category; }

		bool handled = false;
	};

	// ================================================================
	// Event dispatcher
	// ================================================================
	class COCOA EventDispatcher
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>;

	public:
		EventDispatcher(Event& e)
			: mEvent(e)
		{
		}

		template<typename T>
		bool dispatch(EventFn<T> function)
		{
			if (mEvent.getType() == T::getStaticType())
			{
				mEvent.handled = function(*(T*)&mEvent);
				return true;
			}

			return false;
		}

	private:
		Event& mEvent;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.toString();
	}
}

// For convenience other classes only need to include this header
#include "cocoa/events/WindowEvent.h"
#include "cocoa/events/KeyEvent.h"
#include "cocoa/events/MouseEvent.h"
#include "cocoa/events/Input.h"

#pragma warning(pop)

#endif
