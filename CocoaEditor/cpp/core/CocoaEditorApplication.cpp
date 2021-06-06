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
			Settings::General::engineExeDirectory = PathBuilder(File::getExecutableDirectory().getDirectory(-1).c_str());
			Settings::General::engineSourceDirectory = PathBuilder(File::getExecutableDirectory().getDirectory(-4).c_str());
			Logger::Info("Engine executable directory: %s", Settings::General::engineExeDirectory.c_str());

			// Set the assets path as CWD (which should be where the exe is currently located)
			Settings::General::engineAssetsPath = PathBuilder(File::getCwd())
				.join("assets");
			Settings::General::imGuiConfigPath = PathBuilder(Settings::General::engineAssetsPath)
				.join("default.ini");

			// Set the styles directory
			Settings::General::stylesDirectory = PathBuilder(Settings::General::engineAssetsPath)
				.join("styles");

			// Create application store data if it does not exist
			const Path cocoaEngine = PathBuilder(File::getSpecialAppFolder())
				.join("CocoaEngine")
				.createPath();
			File::createDirIfNotExists(cocoaEngine);

			Settings::General::editorSaveData = PathBuilder(cocoaEngine)
				.join(Settings::General::editorSaveData.c_str());
			Settings::General::editorStyleData = PathBuilder(cocoaEngine)
				.join(Settings::General::editorStyleData.c_str());

			// Copy default script files to the assets path
			const Path defaultScriptH = PathBuilder(Settings::General::engineAssetsPath)
				.join("defaultCodeFiles")
				.join("DefaultScript.h")
				.createPath();
			const Path defaultScriptCpp = PathBuilder(Settings::General::engineAssetsPath)
				.join("defaultCodeFiles")
				.join("DefaultScript.cpp")
				.createPath();
			File::copyFile(defaultScriptH, cocoaEngine, "DefaultScript");
			File::copyFile(defaultScriptCpp, cocoaEngine, "DefaultScript");

			String::FreeString(defaultScriptCpp.path);
			String::FreeString(defaultScriptH.path);
			String::FreeString(cocoaEngine.path);
		}

		bool createProject(SceneData& scene, const Path& projectPath, const char* filename)
		{
			StringBuilder projectName;
			projectName.Append(filename);
			projectName.Append(".cprj");
			
			Settings::General::currentProject = PathBuilder(projectPath)
				.join(projectName.c_str());
			Settings::General::currentScene = PathBuilder(projectPath)
				.join("scenes")
				.join("NewScene.cocoa");
			Settings::General::workingDirectory = projectPath;

			json saveData = {
				{"ProjectPath", Settings::General::currentProject.c_str()},
				{"CurrentScene", Settings::General::currentScene.c_str()}
			};

			File::writeFile(saveData.dump(4).c_str(), Settings::General::currentProject.createTmpPath());
			PathBuilder assetsPath = PathBuilder(projectPath)
				.join("assets");
			File::createDirIfNotExists(assetsPath.createTmpPath());
			PathBuilder scriptsPath = PathBuilder(projectPath)
				.join("scripts");
			File::createDirIfNotExists(scriptsPath.createTmpPath());
			PathBuilder scenesPath = PathBuilder(projectPath)
				.join("scenes");
			File::createDirIfNotExists(scenesPath.createTmpPath());

			CocoaEditor* application = (CocoaEditor*)Application::get();
			Scene::freeResources(scene);
			Scene::init(scene);
			Scene::save(scene, Settings::General::currentScene.createTmpPath());
			sourceFileWatcher = std::make_shared<SourceFileWatcher>(scriptsPath.createPath());
			saveEditorData();

			return true;
		}

		void saveEditorData()
		{
			if (projectLoaded)
			{
				ImGui::SaveIniSettingsToDisk(Settings::General::imGuiConfigPath.c_str());
			}

			json saveData = {
				{"ProjectPath", Settings::General::currentProject.c_str()},
				{"EditorStyle", Settings::General::editorStyleData.c_str()},
				{"ImGuiConfig", Settings::General::imGuiConfigPath.c_str()}
			};

			File::writeFile(saveData.dump(4).c_str(), Settings::General::editorSaveData.createTmpPath());
		}

		bool loadEditorData(SceneData& scene, const Path& path)
		{
			FileHandle* editorData = File::openFile(path);
			if (editorData->size > 0)
			{
				json j = json::parse(editorData->data);
				if (!j["EditorStyle"].is_null())
				{
					Settings::General::editorStyleData = j["EditorStyle"].get<std::string>().c_str();
				}

				if (!j["ImGuiConfigPath"].is_null())
				{
					Settings::General::imGuiConfigPath = j["ImGuiConfig"].get<std::string>().c_str();
				}

				if (!j["ProjectPath"].is_null())
				{
					loadProject(scene, PathBuilder(j["ProjectPath"].get<std::string>().c_str()).createTmpPath());
				}
			}
			File::closeFile(editorData);

			return true;
		}

		void saveProject()
		{
			json saveData = {
				{"ProjectPath", Settings::General::currentProject.c_str()},
				{"CurrentScene", Settings::General::currentScene.c_str()},
				{"WorkingDirectory", Settings::General::currentProject.createTmpPath().getDirectory(-1) }
			};

			File::writeFile(saveData.dump(4).c_str(), Settings::General::currentProject.createTmpPath());

			saveEditorData();
		}

		bool loadProject(SceneData& scene, const Path& path)
		{
			Settings::General::currentProject = path;
			FileHandle* projectData = File::openFile(Settings::General::currentProject.createTmpPath());
			bool isLoaded = false;
			if (projectData->size > 0)
			{
				json j = json::parse(projectData->data);
				if (!j["CurrentScene"].is_null())
				{
					Settings::General::currentProject = path;
					Settings::General::currentScene = j["CurrentScene"].get<std::string>().c_str();
					Settings::General::workingDirectory = path.getDirectory(-1).c_str();

					CocoaEditor* application = (CocoaEditor*)Application::get();
					Scene::freeResources(scene);
					Scene::load(scene, Settings::General::currentScene.createTmpPath());

					saveEditorData();
					StringBuilder winTitle;
					winTitle.Append(Settings::General::currentProject.createTmpPath().filename);
					winTitle.Append(" -- ");
					winTitle.Append(Settings::General::currentScene.createTmpPath().filename);
					Application::get()->getWindow()->setTitle(winTitle.c_str());

					const Path scriptsPath = PathBuilder(Settings::General::workingDirectory)
						.join("scripts")
						.createPath();
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
			loadEditorData(scene, Settings::General::editorSaveData.createTmpPath());
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