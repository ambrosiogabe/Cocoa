#pragma once
#include "externalLibs.h"

#include "cocoa/core/Core.h"
#include "cocoa/core/CWindow.h"
#include "cocoa/events/Event.h"
#include "cocoa/events/WindowEvent.h"

namespace Cocoa
{
	class Scene;
	class SceneInitializer;

	typedef void (*AppOnUpdateFn)(Scene* scene, float dt);
	typedef void (*AppOnAttachFn)(Scene* scene);
	typedef void (*AppOnRenderFn)(Scene* scene);
	typedef void (*AppOnEventFn)(Scene* scene, Event& e);

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

		Framebuffer* GetFramebuffer() const;
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
		Framebuffer* m_Framebuffer = nullptr;
		Scene* m_CurrentScene = nullptr;
		CWindow* m_Window;
		ApplicationData m_AppData;
	};

	// To be defined in CLIENT
	Application* CreateApplication();
}