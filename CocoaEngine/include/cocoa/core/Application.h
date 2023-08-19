#ifndef COCOA_ENGINE_APPLICATION_H
#define COCOA_ENGINE_APPLICATION_H
#include "externalLibs.h"

#include "cocoa/core/Core.h"
#include "cocoa/core/CWindow.h"
#include "cocoa/events/Event.h"
#include "cocoa/events/WindowEvent.h"
#include "cocoa/scenes/SceneData.h"

namespace Cocoa
{
	class SceneInitializer;

	typedef void (*AppOnUpdateFn)(SceneData& scene, float dt);
	typedef void (*AppOnAttachFn)(SceneData& scene);
	typedef void (*AppOnRenderFn)(SceneData& scene);
	typedef void (*AppOnEventFn)(SceneData& scene, Event& e);

	/**
	* The main application's DLL data
	* 
	* This data is used by the main executable to call the
	* main events for the application (update, attach, render, event).
	*/
	struct ApplicationData
	{
		AppOnUpdateFn appOnUpdate = nullptr; /**< Callback function to update the app. */
		AppOnAttachFn appOnAttach = nullptr; /**< Callback function to attach the app. */
		AppOnRenderFn appOnRender = nullptr; /**< Callback function to render the app. */
		AppOnEventFn appOnEvent = nullptr; /**< Callback function to forward events to the app. */
	};

	/**
	* The main application class.
	* 
	* This class represents the application and holds is the main entry point for
	* the application. It links into the DLL and calls all the callbacks at the appropriate
	* times.
	*/
	class COCOA Application
	{
	public:
		Application();
		virtual ~Application() = default;

		virtual void init() {}
		virtual void shutdown() {}

		void run();
		void stop();

		virtual void onEvent(Event& e);
		CWindow* getWindow() const;

		static Application* get();

	protected:
		virtual void beginFrame() {}
		virtual void endFrame() {}

	private:
		bool onWindowClose(WindowCloseEvent& e);

		static Application* sInstance;

		bool mRunning;

		float mLastFrameTime = 0;

	protected:
		SceneData mCurrentScene;
		CWindow* mWindow;
		ApplicationData mAppData;
	};

	// To be defined in CLIENT
	Application* createApplication();
}

#endif
