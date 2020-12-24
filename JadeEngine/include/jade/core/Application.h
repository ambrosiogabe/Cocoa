#pragma once
#include "externalLibs.h"

#include "jade/core/Core.h"
#include "jade/core/Layer.h"
#include "jade/core/JWindow.h"
#include "jade/events/Event.h"
#include "jade/events/WindowEvent.h"

namespace Jade
{
	class SceneInitializer;
	class JADE Application
	{
	public:
		Application();
		virtual ~Application();

		virtual void Init() {}
		virtual void Shutdown() {}

		void Run();
		void Stop();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		void PopLayer(Layer* layer);
		void PopOverlay(Layer* overlay);

		virtual void OnEvent(Event& e);

		Framebuffer* GetFramebuffer() const;
		void ChangeScene(SceneInitializer* scene);
		JWindow* GetWindow() const;

		static Application* Get();

	protected:
		virtual void BeginFrame() {}
		virtual void EndFrame() {}

	private:
		bool OnWindowClose(WindowCloseEvent& e);

		static Application* s_Instance;

		bool m_Running;

		// Overlays are always on top of layers
		// So we keep the next index for the next layer handy at all times
		std::vector<Layer*> m_Layers;
		uint32 m_LayerInsertIndex;

		//ImGuiLayer m_ImGuiLayer = ImGuiLayer();

		float m_LastFrameTime = 0;

	protected:
		Framebuffer* m_Framebuffer = nullptr;
		Scene* m_CurrentScene = nullptr;
		JWindow* m_Window;
	};

	// To be defined in CLIENT
	Application* CreateApplication();
}