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
		m_Window = CWindow::Create(1920, 1080, "Test Window");
		s_Instance = this;

		m_Window->SetEventCallback(std::bind(&Application::onEvent, this, std::placeholders::_1));
	}

	void Application::run()
	{
		if (!m_AppData.AppOnAttach) { m_AppData.AppOnAttach = AppOnAttach; }
		if (!m_AppData.AppOnEvent) { m_AppData.AppOnEvent = AppOnEvent; }
		if (!m_AppData.AppOnRender) { m_AppData.AppOnRender = AppOnRender; }
		if (!m_AppData.AppOnUpdate) { m_AppData.AppOnUpdate = AppOnUpdate; }

		m_AppData.AppOnAttach(m_CurrentScene);

		while (mRunning)
		{
			float time = (float)glfwGetTime();
			float dt = time - mLastFrameTime;
			mLastFrameTime = time;

			beginFrame();
			m_AppData.AppOnUpdate(m_CurrentScene, dt);
			m_AppData.AppOnRender(m_CurrentScene);
			endFrame();

			m_Window->OnUpdate();
			m_Window->Render();
		}

		// TODO: Should this be a thing? (probably, add support at some point...)
		//for (Layer* layer : m_Layers)
		//{
		//	layer->OnDetach();
		//}

		m_Window->Destroy();
	}

	bool Application::onWindowClose(WindowCloseEvent& e)
	{
		mRunning = false;
		return true;
	}

	void Application::onEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(std::bind(&Application::onWindowClose, this, std::placeholders::_1));

		m_AppData.AppOnEvent(m_CurrentScene, e);
	}

	void Application::stop()
	{
		mRunning = false;
	}

	CWindow* Application::getWindow() const
	{
		return m_Window;
	}

	Application* Application::s_Instance = nullptr;
	Application* Application::get()
	{
		if (!s_Instance)
		{
			Logger::Error("Cannot get application. It is nullptr.");
		}
		return s_Instance;
	}
}