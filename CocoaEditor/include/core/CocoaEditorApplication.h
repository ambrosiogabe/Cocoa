#pragma once
#include "externalLibs.h"
#include "cocoa/core/Core.h"

#include "core/LevelEditorSystem.h"
#include "editorWindows/ProjectWizard.h"

#include "cocoa/core/Application.h"
#include "cocoa/renderer/Shader.h"
#include "cocoa/core/Handle.h"

namespace Cocoa
{
	class SourceFileWatcher;

	namespace EditorLayer
	{
		void Init();
		void OnAttach(Scene* scene);
		void OnUpdate(Scene* scene, float dt);
		void OnRender(Scene* scene);
		void OnEvent(Scene* scene, Event& e);

		bool CreateProject(Scene* scene, const CPath& projectPath, const char* filename);
		bool LoadEditorData(Scene* scene, const CPath& path);
		bool LoadProject(Scene* scene, const CPath& path);
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