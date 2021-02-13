#pragma once
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

	struct ApplicationData
	{
		AppOnUpdateFn AppOnUpdate = nullptr;
		AppOnAttachFn AppOnAttach = nullptr;
		AppOnRenderFn AppOnRender = nullptr;
		AppOnEventFn AppOnEvent = nullptr;
	};

	class COCOA Application
	{
	public:
		Application();
		virtual ~Application();

		virtual void Init() {}
		virtual void Shutdown() {}

		void Run();
		void Stop();

		virtual void OnEvent(Event& e);

		void ChangeScene(SceneInitializer* scene);
		CWindow* GetWindow() const;

		static Application* Get();

	protected:
		virtual void BeginFrame() {}
		virtual void EndFrame() {}

	private:
		bool OnWindowClose(WindowCloseEvent& e);

		static Application* s_Instance;

		bool m_Running;

		float m_LastFrameTime = 0;

	protected:
		SceneData m_CurrentScene;
		CWindow* m_Window;
		ApplicationData m_AppData;
	};

	// To be defined in CLIENT
	Application* CreateApplication();
}