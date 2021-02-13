#include "externalLibs.h"
#include "cocoa/core/Core.h"

#include "cocoa/core/Application.h"
#include "cocoa/renderer/DebugDraw.h"
#include "cocoa/core/Entity.h"

namespace Cocoa
{
	// Stub methods
	static void AppOnUpdate(SceneData& scene, float dt) { }
	static void AppOnAttach(SceneData& scene) { }
	static void AppOnRender(SceneData& scene) { }
	static void AppOnEvent(SceneData& scene, Event& e) { }

	Application::Application()
	{
		m_Running = true;

		std::string title = std::string("Test Window");
		Log::Info("Initializing GLAD functions in DLL.");
		m_Window = CWindow::Create(1920, 1080, title);
		s_Instance = this;

		m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
	}

	Application::~Application()
	{

	}

	void Application::Run()
	{
		if (!m_AppData.AppOnAttach) { m_AppData.AppOnAttach = AppOnAttach; }
		if (!m_AppData.AppOnEvent) { m_AppData.AppOnEvent = AppOnEvent; }
		if (!m_AppData.AppOnRender) { m_AppData.AppOnRender = AppOnRender; }
		if (!m_AppData.AppOnUpdate) { m_AppData.AppOnUpdate = AppOnUpdate; }

		m_AppData.AppOnAttach(m_CurrentScene);

		while (m_Running)
		{
			float time = (float)glfwGetTime();
			float dt = time - m_LastFrameTime;
			m_LastFrameTime = time;

			BeginFrame();
			m_AppData.AppOnUpdate(m_CurrentScene, dt);
			m_AppData.AppOnRender(m_CurrentScene);
			EndFrame();

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

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(std::bind(&Application::OnWindowClose, this, std::placeholders::_1));

		m_AppData.AppOnEvent(m_CurrentScene, e);
	}

	void Application::ChangeScene(SceneInitializer* sceneInitializer)
	{
		if (m_CurrentScene.CurrentSceneInitializer != nullptr)
		{
			delete m_CurrentScene.CurrentSceneInitializer;
		}

		if (m_CurrentScene.SceneCamera != nullptr)
		{
			delete m_CurrentScene.SceneCamera;
		}

		m_CurrentScene = Scene::Create(sceneInitializer);
		Scene::Init(m_CurrentScene);
		Scene::Start(m_CurrentScene);
		
		DebugDraw::Init(&m_CurrentScene);
	}

	void Application::Stop()
	{
		m_Running = false;
	}

	CWindow* Application::GetWindow() const
	{
		return m_Window;
	}

	Application* Application::s_Instance = nullptr;
	Application* Application::Get()
	{
		if (!s_Instance)
		{
			Log::Error("Cannot get application. It is nullptr.");
		}
		return s_Instance;
	}
}