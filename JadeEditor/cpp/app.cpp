#include "Jade.h"

#include "LevelEditorSystem.h"
#include "LevelEditorScene.h"
#include "ImGuiLayer.h"
#include "jade/file/IFile.h"
#include "jade/util/Settings.h"

#include <glad/glad.h>
#include <jsonVendor/json.hpp>

namespace Jade
{
	class EditorLayer : public Layer
	{
	public:
		EditorLayer()
		{
			Application::Get()->ChangeScene(new LevelEditorScene());
		}

		virtual void OnAttach() override
		{
			IFile::CreateDirIfNotExists((IFile::GetSpecialAppFolder() + "\\JadeEngine").c_str());

			Settings::General::s_EditorSaveData = IFile::GetSpecialAppFolder() + "\\JadeEngine\\" + Settings::General::s_EditorSaveData;
			Settings::General::s_EditorStyleData = IFile::GetSpecialAppFolder() + "\\JadeEngine\\" + Settings::General::s_EditorStyleData;
			File* editorData = IFile::OpenFile(Settings::General::s_EditorSaveData.c_str());
			if (editorData->m_Data.size() > 0)
			{
				json j = json::parse(editorData->m_Data);
				if (!j["CurrentProject"].is_null())
				{
					Settings::General::s_CurrentProject = j["CurrentProject"];
				}

				if (!j["CurrentScene"].is_null())
				{
					Settings::General::s_CurrentScene = j["CurrentScene"];
					Application::Get()->GetScene()->Load(Settings::General::s_CurrentScene);
				}

				if (!j["EditorStyle"].is_null())
				{
					Settings::General::s_EditorStyleData = j["EditorStyle"];
				}
			}
			IFile::CloseFile(editorData);
		}

		virtual void OnUpdate(float dt) override
		{
			DebugDraw::BeginFrame();
			Application::Get()->GetScene()->Update(dt);
		}

		virtual void OnImGuiRender() override
		{
			Application::Get()->GetScene()->ImGui();
		}

		virtual void OnRender() override
		{
			glBindFramebuffer(GL_FRAMEBUFFER, Application::Get()->GetFramebuffer()->GetId());
			glViewport(0, 0, 3840, 2160);
			glClearColor(0.45f, 0.55f, 0.6f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			DebugDraw::DrawBottomBatches();
			Application::Get()->GetScene()->Render();
			DebugDraw::DrawTopBatches();
		}

		virtual void OnEvent(Event& e) override
		{
			const auto& systems = Application::Get()->GetScene()->GetSystems();

			for (const auto& system : systems)
			{
				system->OnEvent(e);
			}
		}
	};

	class JadeEditor : public Application
	{
	public:
		~JadeEditor() = default;
		JadeEditor()
		{
			PushOverlay(&m_ImGuiLayer);
		}

		virtual void BeginFrame() override
		{
			m_ImGuiLayer.BeginFrame();
		}

		virtual void EndFrame() override
		{
			m_ImGuiLayer.EndFrame();
		}

	private:
		ImGuiLayer m_ImGuiLayer;
	};


	Application* CreateApplication()
	{
		JadeEditor* editor = new JadeEditor();
		editor->PushLayer(new EditorLayer());
		return editor;
	}
}