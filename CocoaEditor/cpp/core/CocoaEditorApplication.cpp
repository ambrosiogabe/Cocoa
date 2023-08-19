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

#include <filesystem>

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
			std::filesystem::path exeDirectory = File::getExecutableDirectory();
			Settings::General::engineExeDirectory = exeDirectory.parent_path();
			Settings::General::engineSourceDirectory = exeDirectory.parent_path();
			Logger::Info("Current working directory: %s", File::getCwd().string().c_str());
			Logger::Info("Engine executable directory: %s", Settings::General::engineExeDirectory.string().c_str());
			Logger::Info("Engine source directory: %s", Settings::General::engineSourceDirectory.string().c_str());

			// Set the assets path as CWD (which should be where the exe is currently located)
			Settings::General::engineAssetsPath = File::getCwd()/"assets";
			Settings::General::imGuiConfigPath = Settings::General::engineAssetsPath/"default.ini";
			Logger::Info("Engine assets path: %s", Settings::General::engineAssetsPath.string().c_str());
			Logger::Info("Imgui Config path: %s", Settings::General::imGuiConfigPath.string().c_str());

			// Set the styles directory
			Settings::General::stylesDirectory = Settings::General::engineAssetsPath / "styles";

			// Create application store data if it does not exist
			const std::filesystem::path cocoaEngine = File::getSpecialAppFolder() / "CocoaEngine";
			File::createDirIfNotExists(cocoaEngine);

			Settings::General::editorSaveData = cocoaEngine / Settings::General::editorSaveData;
			Settings::General::editorStyleData = cocoaEngine / Settings::General::editorStyleData;

			// Copy default script files to the assets path
			const std::filesystem::path defaultScriptH =
				Settings::General::engineAssetsPath / "defaultCodeFiles" / "DefaultScript.h";
			const std::filesystem::path defaultScriptCpp =
				Settings::General::engineAssetsPath / "defaultCodeFiles" / "DefaultScript.cpp";
			File::copyFile(defaultScriptH, cocoaEngine, "DefaultScript");
			File::copyFile(defaultScriptCpp, cocoaEngine, "DefaultScript");
		}

		bool createProject(SceneData& scene, const std::filesystem::path& projectPath, const char* filename)
		{
			StringBuilder projectName;
			projectName.Append(filename);
			projectName.Append(".cprj");

			Settings::General::currentProject = projectPath / projectName.c_str();
			Settings::General::currentScene = projectPath / "scenes" / "NewScene.cocoa";
			Settings::General::workingDirectory = projectPath;

			json saveData = {
				{"ProjectPath", Settings::General::currentProject.string()},
				{"CurrentScene", Settings::General::currentScene.string()}
			};

			File::writeFile(saveData.dump(4).c_str(), Settings::General::currentProject);
			std::filesystem::path assetsPath = projectPath / "assets";
			File::createDirIfNotExists(assetsPath);
			std::filesystem::path scriptsPath = projectPath / "scripts";
			File::createDirIfNotExists(scriptsPath);
			std::filesystem::path scenesPath = projectPath / "scenes";
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
				ImGui::SaveIniSettingsToDisk(Settings::General::imGuiConfigPath.string().c_str());
			}

			json saveData = {
				{"ProjectPath", Settings::General::currentProject.string()},
				{"EditorStyle", Settings::General::editorStyleData.string()},
				{"ImGuiConfig", Settings::General::imGuiConfigPath.string()}
			};

			File::writeFile(saveData.dump(4).c_str(), Settings::General::editorSaveData);
		}

		bool loadEditorData(SceneData& scene, const std::filesystem::path& path)
		{
			FileHandle* editorData = File::openFile(path);
			if (editorData->size > 0)
			{
				json j = json::parse(editorData->data);
				if (!j["EditorStyle"].is_null())
				{
					Settings::General::editorStyleData = j["EditorStyle"].get<std::string>();
				}

				if (!j["ImGuiConfigPath"].is_null())
				{
					Settings::General::imGuiConfigPath = j["ImGuiConfig"].get<std::string>();
				}

				if (!j["ProjectPath"].is_null())
				{
					loadProject(scene, j["ProjectPath"].get<std::string>());
				}
			}
			File::closeFile(editorData);

			return true;
		}

		void saveProject()
		{
			json saveData = {
				{"ProjectPath", Settings::General::currentProject.string()},
				{"CurrentScene", Settings::General::currentScene.string()},
				{"WorkingDirectory", Settings::General::currentProject.parent_path().string()}
			};

			File::writeFile(saveData.dump(4).c_str(), Settings::General::currentProject);

			saveEditorData();
		}

		bool loadProject(SceneData& scene, const std::filesystem::path& path)
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
					Settings::General::currentScene = j["CurrentScene"].get<std::string>();
					Settings::General::workingDirectory = path.parent_path();

					CocoaEditor* application = (CocoaEditor*)Application::get();
					Scene::freeResources(scene);
					Scene::load(scene, Settings::General::currentScene);

					saveEditorData();
					std::string winTitle = Settings::General::currentProject.filename().string();
					winTitle += " -- " + Settings::General::currentScene.filename().string();
					Application::get()->getWindow()->setTitle(winTitle.c_str());

					const std::filesystem::path scriptsPath = Settings::General::workingDirectory / "scripts";
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
		// Initialize memory tracker. All memory allocated after this line will be tracked.
		Memory::init();

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
		Memory::dumpMemoryLeaks();
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
		//CXIndex index = clang_createIndex(0, 0);
		//CXTranslationUnit unit = clang_parseTranslationUnit(
		//	index,
		//	"C:\\Users\\Gabe\\Downloads\\test.hpp", nullptr, 0,
		//	nullptr, 0,
		//	CXTranslationUnit_None
		//);
		//if (unit == nullptr)
		//{
		//	printf("Unable to parse translation unit.");
		//	exit(-1);
		//}

		//CXCursor cursor = clang_getTranslationUnitCursor(unit);
		//clang_visitChildren(
		//	cursor,
		//	[](CXCursor c, CXCursor parent, CXClientData clientData)
		//	{
		//		//printf("Cursor '");
		//		//printCXString(clang_getCursorSpelling(c));
		//		//printf("' of kind '");
		//		//printCXString(clang_getCursorKindSpelling(clang_getCursorKind(c)));
		//		//printf("'\n");
		//		return CXChildVisit_Recurse;
		//	},
		//	nullptr
		//);

		//clang_disposeTranslationUnit(unit);
		//clang_disposeIndex(index);

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