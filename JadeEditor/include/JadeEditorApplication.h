#include "LevelEditorSystem.h"
#include "ProjectWizard.h"

#include "jade/core/Layer.h"
#include "jade/core/Application.h"

namespace Jade
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer()
		{
		}

		virtual void OnAttach() override;
		virtual void OnUpdate(float dt) override;
		virtual void OnImGuiRender() override;
		virtual void OnRender() override;
		virtual void OnEvent(Event& e) override;

		static bool CreateProject(const JPath& projectPath, const char* filename);
		static bool LoadEditorData(const JPath& path);
		static bool LoadProject(const JPath& path);
		static void SaveEditorData();
		static void SaveProject();
		inline void SetProjectLoaded() { m_ProjectLoaded = true; }
		inline bool IsProjectLoaded() { return m_ProjectLoaded; }

	private:
		ProjectWizard m_ProjectWizard;

	private:
		bool m_ProjectLoaded = false;
	};

	class ImGuiLayer;
	class JadeEditor : public Application
	{
	public:
		~JadeEditor() = default;
		JadeEditor();

		virtual void BeginFrame() override;
		virtual void EndFrame() override;

		const EditorLayer& GetEditorLayer() { return m_EditorLayer; }

		static bool IsProjectLoaded()
		{
			JadeEditor* app = static_cast<JadeEditor*>(Application::Get());
			return app->m_EditorLayer.IsProjectLoaded();
		}

		static void SetProjectLoaded()
		{
			JadeEditor* app = static_cast<JadeEditor*>(Application::Get());
			app->m_EditorLayer.SetProjectLoaded();
		}

	private:
		EditorLayer m_EditorLayer;
		ImGuiLayer* m_ImGuiLayer;
	};
}