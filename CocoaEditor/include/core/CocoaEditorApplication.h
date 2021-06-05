#ifndef COCOA_EDITOR_COCOA_EDITOR_APPLICATION_H
#define COCOA_EDITOR_COCOA_EDITOR_APPLICATION_H
#include "core/LevelEditorSystem.h"

#include "cocoa/core/Application.h"
#include "cocoa/scenes/SceneData.h"

namespace Cocoa
{
	class SourceFileWatcher;

	namespace EditorLayer
	{
		void init();
		void onAttach(SceneData& scene);
		void onUpdate(SceneData& scene, float dt);
		void onRender(SceneData& scene);
		void onEvent(SceneData& scene, Event& e);

		bool createProject(SceneData& scene, const CPath& projectPath, const char* filename);
		bool loadEditorData(SceneData& scene, const CPath& path);
		bool loadProject(SceneData& scene, const CPath& path);
		void saveEditorData();
		void saveProject();
		void setProjectLoaded();
		bool isProjectLoaded();
	};

	class CocoaEditor : public Application
	{
	public:
		~CocoaEditor() = default;
		CocoaEditor();

		void beginFrame() override;
		void endFrame() override;
		void init() override;
		void shutdown() override;

		static bool isProjectLoaded()
		{
			return EditorLayer::isProjectLoaded();
		}

		static void setProjectLoaded()
		{
			EditorLayer::setProjectLoaded();
		}

		void setAppData(AppOnAttachFn attachFn, AppOnUpdateFn updateFn, AppOnRenderFn renderFn, AppOnEventFn eventFn);
	};
}

#endif
