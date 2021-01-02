#include "LevelEditorSystem.h"
#include "ProjectWizard.h"
#include "PickingTexture.h"

#include "cocoa/core/Layer.h"
#include "cocoa/core/Application.h"
#include "cocoa/renderer/Shader.h"

namespace Cocoa
{
	class SourceFileWatcher;

	class EditorLayer : public Layer
	{
	public:
		EditorLayer(Scene* scene);

		inline void SetScene(Scene* scene) { this->m_Scene = scene; }
		virtual void OnAttach() override;
		virtual void OnUpdate(float dt) override;
		virtual void OnRender() override;
		virtual void OnEvent(Event& e) override;

		static bool CreateProject(const CPath& projectPath, const char* filename);
		static bool LoadEditorData(const CPath& path);
		static bool LoadProject(const CPath& path);
		static void SaveEditorData();
		static void SaveProject();
		inline void SetProjectLoaded() { m_ProjectLoaded = true; }
		inline bool IsProjectLoaded() { return m_ProjectLoaded; }

		inline uint32 GetPickingTextureID() const { return m_PickingTexture.GetPickingTextureID(); }
		inline const PickingTexture& GetPickingTexture() const { return m_PickingTexture; }

	private:
		PickingTexture m_PickingTexture;
		std::shared_ptr<Shader> m_PickingShader;
		std::shared_ptr<Shader> m_DefaultShader;
		std::shared_ptr<Shader> m_OutlineShader;
		std::shared_ptr<SourceFileWatcher> m_SourceFileWatcher;

	private:
		bool m_ProjectLoaded = false;
		bool m_EditorUpdate = true;
	};

	class ImGuiLayer;

	class CocoaEditor : public Application
	{
	public:
		~CocoaEditor() = default;
		CocoaEditor();

		virtual void BeginFrame() override;
		virtual void EndFrame() override;
		virtual void Init() override;
		virtual void Shutdown() override;

		const EditorLayer* GetEditorLayer() { return m_EditorLayer; }

		static bool IsProjectLoaded()
		{
			CocoaEditor* app = static_cast<CocoaEditor*>(Application::Get());
			return app->m_EditorLayer->IsProjectLoaded();
		}

		static void SetProjectLoaded()
		{
			CocoaEditor* app = static_cast<CocoaEditor*>(Application::Get());
			app->m_EditorLayer->SetProjectLoaded();
		}

	private:
		EditorLayer* m_EditorLayer;
		ImGuiLayer* m_ImGuiLayer;
	};
}