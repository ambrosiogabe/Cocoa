#include "Cocoa.h"

#include "core/CocoaEditorApplication.h"
#include "core/LevelEditorSceneInitializer.h"
#include "core/ImGuiLayer.h"
#include "gui/ImGuiHeader.h"
#include "editorWindows/ProjectWizard.h"
#include "nativeScripting/SourceFileWatcher.h"
#include "renderer/Gizmos.h"

#include "cocoa/file/File.h"
#include "cocoa/util/Settings.h"
#include "cocoa/core/AssetManager.h"

#include <glad/glad.h>
#include <nlohmann/json.hpp>

// TODO: REMOVE ME
#include <clang-c/Index.h>

#ifdef CopyFile
#undef CopyFile
#endif

namespace Cocoa
{
	// ===================================================================================
	// Editor Layer
	// ===================================================================================
	namespace EditorLayer
	{
		// Internal Variables
		static std::shared_ptr<SourceFileWatcher> sourceFileWatcher;
		static bool projectLoaded = false;
		static bool editorUpdate = true;

		// Forward Declarations

		void init()
		{
			Settings::General::engineExeDirectory = CPath::create(File::getExecutableDirectory().getDirectory(-1));
			Settings::General::engineSourceDirectory = CPath::create(File::getExecutableDirectory().getDirectory(-4));
			Logger::Info("%s", Settings::General::engineExeDirectory.path);

			// Set the assets path as CWD (which should be where the exe is currently located)
			Settings::General::engineAssetsPath = File::getCwd();
			Settings::General::engineAssetsPath.join(CPath::create("assets"));
			Settings::General::imGuiConfigPath = Settings::General::engineAssetsPath;
			Settings::General::imGuiConfigPath.join(CPath::create("default.ini"));

			// Set the styles directory
			Settings::General::stylesDirectory = Settings::General::engineAssetsPath;
			Settings::General::stylesDirectory.join(CPath::create("styles"));

			// Create application store data if it does not exist
			CPath cocoaEngine = File::getSpecialAppFolder();
			cocoaEngine.join(CPath::create("CocoaEngine"));
			File::createDirIfNotExists(cocoaEngine);

			CPath editorSaveData = cocoaEngine;
			editorSaveData.join(Settings::General::editorSaveData);
			Settings::General::editorSaveData = editorSaveData;
			CPath editorStyleData = cocoaEngine;
			editorStyleData.join(Settings::General::editorStyleData);
			Settings::General::editorStyleData = editorStyleData;

			// Copy default script files to the assets path
			CPath defaultScriptH = Settings::General::engineAssetsPath;
			defaultScriptH.join(CPath::create("defaultCodeFiles"));
			defaultScriptH.join(CPath::create("DefaultScript.h"));
			CPath defaultScriptCpp = Settings::General::engineAssetsPath;
			defaultScriptCpp.join(CPath::create("defaultCodeFiles"));
			defaultScriptCpp.join(CPath::create("DefaultScript.cpp"));
			File::copyFile(defaultScriptH, cocoaEngine, "DefaultScript");
			File::copyFile(defaultScriptCpp, cocoaEngine, "DefaultScript");
		}

		bool createProject(SceneData& scene, const CPath& projectPath, const char* filename)
		{
			Settings::General::currentProject = projectPath;
			Settings::General::currentProject.join(CPath::create(std::string(filename) + ".cprj"));
			Settings::General::currentScene = projectPath;
			Settings::General::currentScene.join(CPath::create("scenes"));
			Settings::General::currentScene.join(CPath::create("NewScene.cocoa"));
			Settings::General::workingDirectory = CPath::create(Settings::General::currentProject.getDirectory(-1));

			json saveData = {
				{"ProjectPath", Settings::General::currentProject.path},
				{"CurrentScene", Settings::General::currentScene.path}
			};

			File::writeFile(saveData.dump(4).c_str(), Settings::General::currentProject);
			CPath assetsPath = projectPath;
			assetsPath.join(CPath::create("assets"));
			File::createDirIfNotExists(assetsPath);
			CPath scriptsPath = projectPath;
			scriptsPath.join(CPath::create("scripts"));
			File::createDirIfNotExists(scriptsPath);
			CPath scenesPath = projectPath;
			scenesPath.join(CPath::create("scenes"));
			File::createDirIfNotExists(scenesPath);

			CocoaEditor* application = (CocoaEditor*)Application::get();
			Scene::freeResources(scene);
			Scene::init(scene);
			Scene::save(scene, Settings::General::currentScene);
			sourceFileWatcher = std::make_shared<SourceFileWatcher>(scriptsPath);
			saveEditorData();

			return true;
		}

		void saveEditorData()
		{
			if (projectLoaded)
			{
				ImGui::SaveIniSettingsToDisk(Settings::General::imGuiConfigPath.path);
			}

			json saveData = {
				{"ProjectPath", Settings::General::currentProject.path},
				{"EditorStyle", Settings::General::editorStyleData.path},
				{"ImGuiConfig", Settings::General::imGuiConfigPath.path}
			};

			File::writeFile(saveData.dump(4).c_str(), Settings::General::editorSaveData);
		}

		bool loadEditorData(SceneData& scene, const CPath& path)
		{
			FileHandle* editorData = File::openFile(path);
			if (editorData->size > 0)
			{
				json j = json::parse(editorData->data);
				if (!j["EditorStyle"].is_null())
				{
					Settings::General::editorStyleData = CPath::create(j["EditorStyle"], false);
				}

				if (!j["ImGuiConfigPath"].is_null())
				{
					Settings::General::imGuiConfigPath = CPath::create(j["ImGuiConfig"], false);
				}

				if (!j["ProjectPath"].is_null())
				{
					loadProject(scene, CPath::create(j["ProjectPath"], false));
				}
			}
			File::closeFile(editorData);

			return true;
		}

		void saveProject()
		{
			json saveData = {
				{"ProjectPath", Settings::General::currentProject.path},
				{"CurrentScene", Settings::General::currentScene.path},
				{"WorkingDirectory", Settings::General::currentProject.getDirectory(-1) }
			};

			File::writeFile(saveData.dump(4).c_str(), Settings::General::currentProject);

			saveEditorData();
		}

		bool loadProject(SceneData& scene, const CPath& path)
		{
			Settings::General::currentProject = path;
			FileHandle* projectData = File::openFile(Settings::General::currentProject);
			bool isLoaded = false;
			if (projectData->size > 0)
			{
				json j = json::parse(projectData->data);
				if (!j["CurrentScene"].is_null())
				{
					Settings::General::currentProject = path;
					Settings::General::currentScene = CPath::create(j["CurrentScene"], false);
					Settings::General::workingDirectory = CPath::create(path.getDirectory(-1));

					CocoaEditor* application = (CocoaEditor*)Application::get();
					Scene::freeResources(scene);
					Scene::load(scene, Settings::General::currentScene);

					saveEditorData();
					StringBuilder winTitle;
					winTitle.Append(Settings::General::currentProject.filename());
					winTitle.Append(" -- ");
					winTitle.Append(Settings::General::currentScene.filename());
					Application::get()->getWindow()->setTitle(winTitle.c_str());

					CPath scriptsPath = Settings::General::workingDirectory;
					scriptsPath.join(CPath::create("scripts"));
					File::getFoldersInDir(scriptsPath);
					sourceFileWatcher = std::make_shared<SourceFileWatcher>(scriptsPath);
					static_cast<CocoaEditor*>(Application::get())->setProjectLoaded();
					isLoaded = true;
				}
			}

			File::closeFile(projectData);
			return isLoaded;
		}

		void onAttach(SceneData& scene)
		{
			loadEditorData(scene, Settings::General::editorSaveData);
		}

		void onUpdate(SceneData& scene, float dt)
		{
			if (scene.isPlaying)
			{
				editorUpdate = false;
			}
			else
			{
				editorUpdate = true;
			}

			if (CocoaEditor::isProjectLoaded() && !editorUpdate)
			{
				DebugDraw::beginFrame();
				Scene::update(scene, dt);
			}
			else if (CocoaEditor::isProjectLoaded())
			{
				DebugDraw::beginFrame();
				Scene::editorUpdate(scene, dt);
				LevelEditorSystem::editorUpdate(scene, dt);
				GizmoSystem::editorUpdate(scene, dt);

				// TODO: It doesn't *really* matter where we put the imgui as long as it gets called... consider creating a dedicated imgui function for 
				// TODO: applications though...
				GizmoSystem::imgui();
			}
		}

		void onRender(SceneData& scene)
		{
			if (CocoaEditor::isProjectLoaded())
			{
				Scene::render(scene);
			}
		}

		void onEvent(SceneData& scene, Event& e)
		{
			if (CocoaEditor::isProjectLoaded())
			{
				Scene::onEvent(scene, e);
				// Order matters here. We want to do the top most layer to the bottom most so that events are blocked
				// if needed
				// TODO: Come up with better solution then if checks constantly. (Maybe abstract this into another function?)
				if (!e.handled)
				{
					ImGuiLayer::onEvent(scene, e);
				}
				if (!e.handled)
				{
					LevelEditorSystem::onEvent(scene, e);
				}
				if (!e.handled)
				{
					GizmoSystem::onEvent(scene, e);
				}
			}
		}

		void setProjectLoaded()
		{
			projectLoaded = true;
		}

		bool isProjectLoaded()
		{
			return projectLoaded; 
		}
	}

	// ===================================================================================
	// Editor Application
	// ===================================================================================
	CocoaEditor::CocoaEditor()
		: Application()
	{
	}

	void CocoaEditor::init()
	{
		// Initialize GLAD here, so that it works in DLL and exe
		Logger::Info("Initializing GLAD functions in exe.");
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			Logger::Error("Unable to initialize GLAD.");
		}

		// Engine initialization
		AssetManager::init(0);
		ProjectWizard::init();
		Input::init();

		// Application Initialization
		EditorLayer::init();
		ImGuiLayer::init(getWindow()->getNativeWindow());

		mCurrentScene = Scene::create(new LevelEditorSceneInitializer());
		Scene::init(mCurrentScene);
		Scene::start(mCurrentScene);
		DebugDraw::init();
	}

	void CocoaEditor::shutdown()
	{
		// Engine shutdown sequence... Well maybe we'll add more here at another time
		// maybe not because at this point the OS should reclaim all resources. But this
		// would be a good place to put in saving anything that wasn't saved already
		// TODO: Try to save temporary files just in case the engine shutdown prematurely

#if _COCOA_DEBUG
		// In debug builds free all the memory to make sure there are no leaks
		ImGuiLayer::destroy();
		DebugDraw::destroy();
		Scene::freeResources(mCurrentScene);
#endif
		
		// This won't really do anything in release builds
		Memory::DumpMemoryLeaks();
	}

	void CocoaEditor::beginFrame()
	{
		ImGuiLayer::beginFrame(mCurrentScene);
	}

	void CocoaEditor::endFrame()
	{
		ImGuiLayer::endFrame();
	}

	void CocoaEditor::setAppData(AppOnAttachFn attachFn, AppOnUpdateFn updateFn, AppOnRenderFn renderFn, AppOnEventFn eventFn)
	{
		mAppData.appOnAttach = attachFn;
		mAppData.appOnUpdate = updateFn;
		mAppData.appOnRender = renderFn;
		mAppData.appOnEvent = eventFn;
	}

	// TODO: Remove me
	static void printCXString(const CXString& str)
	{
		const char* cStr = clang_getCString(str);
		printf("%s", cStr);
		clang_disposeString(str);
	}

	// ===================================================================================
	// Create application entry point
	// ===================================================================================
	Application* createApplication()
	{
		CXIndex index = clang_createIndex(0, 0);
		CXTranslationUnit unit = clang_parseTranslationUnit(
			index,
			"C:\\Users\\Gabe\\Downloads\\test.hpp", nullptr, 0,
			nullptr, 0,
			CXTranslationUnit_None
		);
		if (unit == nullptr)
		{
			printf("Unable to parse translation unit.");
			exit(-1);
		}

		CXCursor cursor = clang_getTranslationUnitCursor(unit);
		clang_visitChildren(
			cursor,
			[](CXCursor c, CXCursor parent, CXClientData clientData)
			{
				//printf("Cursor '");
				//printCXString(clang_getCursorSpelling(c));
				//printf("' of kind '");
				//printCXString(clang_getCursorKindSpelling(clang_getCursorKind(c)));
				//printf("'\n");
				return CXChildVisit_Recurse;
			},
			nullptr
		);

		clang_disposeTranslationUnit(unit);
		clang_disposeIndex(index);

		CocoaEditor* editor = new CocoaEditor();
		editor->setAppData(
			EditorLayer::onAttach,
			EditorLayer::onUpdate,
			EditorLayer::onRender,
			EditorLayer::onEvent
		);
		return editor;
	}
}