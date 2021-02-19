#pragma once
#include "externalLibs.h"
#include "cocoa/core/Core.h"

#include "core/LevelEditorSystem.h"
#include "editorWindows/ProjectWizard.h"

#include "cocoa/core/Application.h"
#include "cocoa/core/Handle.h"
#include "cocoa/renderer/Shader.h"
#include "cocoa/scenes/SceneData.h"

namespace Cocoa
{
	class SourceFileWatcher;

	namespace EditorLayer
	{
		void Init();
		void OnAttach(SceneData& scene);
		void OnUpdate(SceneData& scene, float dt);
		void OnRender(SceneData& scene);
		void OnEvent(SceneData& scene, Event& e);

		bool CreateProject(SceneData& scene, const CPath& projectPath, const char* filename);
		bool LoadEditorData(SceneData& scene, const CPath& path);
		bool LoadProject(SceneData& scene, const CPath& path);
		void SaveEditorData();
		void SaveProject();
		void SetProjectLoaded();
		bool IsProjectLoaded();
	};

	class CocoaEditor : public Application
	{
	public:
		~CocoaEditor() = default;
		CocoaEditor();

		virtual void BeginFrame() override;
		virtual void EndFrame() override;
		virtual void Init() override;
		virtual void Shutdown() override;

		static bool IsProjectLoaded()
		{
			return EditorLayer::IsProjectLoaded();
		}

		static void SetProjectLoaded()
		{
			EditorLayer::SetProjectLoaded();
		}

		void SetAppData(AppOnAttachFn attachFn, AppOnUpdateFn updateFn, AppOnRenderFn renderFn, AppOnEventFn eventFn);
	};
}