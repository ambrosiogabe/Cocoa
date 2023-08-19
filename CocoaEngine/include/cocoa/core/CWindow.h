#ifndef COCOA_ENGINE_CWINDOW_H
#define COCOA_ENGINE_CWINDOW_H
#include "externalLibs.h"

#include "cocoa/core/Core.h"
#include "cocoa/events/Event.h"

namespace Cocoa
{
	class COCOA CWindow
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		CWindow(uint32 width, uint32 height, const std::string& name);

		void init(uint32 width, uint32 height, const std::string& name);
		void onUpdate();

		// Window Attributes
		void setVSync(bool enabled);
		bool isVSync() const;

		void* getNativeWindow() const;

		static CWindow* create(uint32 width, uint32 height, const std::string& name);

		bool isRunning();
		void setEventCallback(const EventCallbackFn& callback);
		glm::vec2 getWindowPos();

		void setWidth(const int newWidth) { mSize.x = (float)newWidth; }
		void setHeight(const int newHeight) { mSize.y = (float)newHeight; }
		void setSize(const glm::vec2& size);
		void setTitle(const char* newTitle);
		glm::vec2 getMonitorSize();
		int getWidth() const { return (int)mSize.x; }
		int getHeight() const { return (int)mSize.y; }
		const glm::vec2& getSize() const { return mSize; }

		float getTargetAspectRatio() const { return mTargetAspectRatio; }
		void render();

		void destroy();

	private:
		CWindow() = default;

	private:
		void* mWindowHandle = nullptr;

		EventCallbackFn mEventCallback = nullptr;
		bool mVSync = false;
		bool mRunning = true;
		glm::vec2 mSize{ 1920, 1080 };
		float mTargetAspectRatio = 3840.0f / 2160.0f;
	};
}

#endif
