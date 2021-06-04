#include "externalLibs.h"
#include "cocoa/core/Core.h"

#include "cocoa/core/Application.h"

namespace Cocoa
{
	// Stub methods
	static void AppOnUpdate(SceneData& scene, float dt) { }
	static void AppOnAttach(SceneData& scene) { }
	static void AppOnRender(SceneData& scene) { }
	static void AppOnEvent(SceneData& scene, Event& e) { }

	Application::Application()
	{
		mRunning = true;

		Logger::Info("Initializing GLAD functions in DLL.");
		mWindow = CWindow::create(1920, 1080, "Test Window");
		sInstance = this;

		mWindow->setEventCallback(std::bind(&Application::onEvent, this, std::placeholders::_1));
	}

	void Application::run()
	{
		if (!mAppData.appOnAttach) { mAppData.appOnAttach = AppOnAttach; }
		if (!mAppData.appOnEvent) { mAppData.appOnEvent = AppOnEvent; }
		if (!mAppData.appOnRender) { mAppData.appOnRender = AppOnRender; }
		if (!mAppData.appOnUpdate) { mAppData.appOnUpdate = AppOnUpdate; }

		mAppData.appOnAttach(mCurrentScene);

		while (mRunning)
		{
			float time = (float)glfwGetTime();
			float dt = time - mLastFrameTime;
			mLastFrameTime = time;

			beginFrame();
			mAppData.appOnUpdate(mCurrentScene, dt);
			mAppData.appOnRender(mCurrentScene);
			endFrame();

			mWindow->onUpdate();
			mWindow->render();
		}

		// TODO: Should this be a thing? (probably, add support at some point...)
		//for (Layer* layer : m_Layers)
		//{
		//	layer->OnDetach();
		//}

		mWindow->destroy();
	}

	bool Application::onWindowClose(WindowCloseEvent& e)
	{
		mRunning = false;
		return true;
	}

	void Application::onEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<WindowCloseEvent>(std::bind(&Application::onWindowClose, this, std::placeholders::_1));

		mAppData.appOnEvent(mCurrentScene, e);
	}

	void Application::stop()
	{
		mRunning = false;
	}

	CWindow* Application::getWindow() const
	{
		return mWindow;
	}

	Application* Application::sInstance = nullptr;
	Application* Application::get()
	{
		if (!sInstance)
		{
			Logger::Error("Cannot get application. It is nullptr.");
		}
		return sInstance;
	}
}