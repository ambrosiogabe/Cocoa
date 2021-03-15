#include "cocoa/systems/ScriptSystem.h"
#include "cocoa/file/File.h"
#include "cocoa/util/Settings.h"

#ifdef _WIN32
#include <windows.h>
#include <libloaderapi.h>
#endif

namespace Cocoa
{
	namespace ScriptSystem
	{
		// Internal Variables
		static AddComponentFn m_AddComponentFromString = nullptr;
		static UpdateScriptFn m_UpdateScripts = nullptr;
		static EditorUpdateScriptFn m_EditorUpdateScripts = nullptr;
		static SaveScriptsFn m_SaveScripts = nullptr;
		static LoadScriptFn m_LoadScript = nullptr;
		static InitScriptsFn m_InitScripts = nullptr;
		static InitImGuiFn m_InitImGui = nullptr;
		static ImGuiFn m_ImGui = nullptr;
		
		static bool m_IsLoaded = false;
		static HMODULE m_Module;

		// Forward Declarations
		static void AddComponentStub(entt::registry&, std::string, entt::entity) { Log::Warning("Adding component from STUB"); }
		static void UpdateScriptStub(entt::registry&, float) {}
		static void EditorUpdateScriptStub(entt::registry&, float) {}
		static void SaveScriptsStub(entt::registry&, json&, SceneData*) {}
		static void LoadScriptStub(entt::registry&, json&, Entity) {}
		static void InitScriptsStub(SceneData*) {}
		static void InitImGuiStub(void*) {}
		static void ImGuiStub(entt::registry&, Entity) {}

		static FARPROC __stdcall TryLoadFunction(HMODULE module, const char* functionName)
		{
			auto func = GetProcAddress(module, functionName);
			if (func == NULL)
			{
				Log::Warning("Could not load dll function '%s'", functionName);
			}

			return func;
		}

		void Init(SceneData& scene)
		{
			Reload(scene);
		}

		void Update(SceneData& scene, float dt)
		{
			if (m_UpdateScripts)
			{
				m_UpdateScripts(scene.Registry, dt);
			}
		}

		void EditorUpdate(SceneData& scene, float dt)
		{
			if (m_EditorUpdateScripts)
			{
				m_EditorUpdateScripts(scene.Registry, dt);
			}
		}

		void Reload(SceneData& scene)
		{
			if (m_IsLoaded)
			{
				if (!FreeScriptLibrary(scene)) return;
			}

			CPath scriptDllPath = Settings::General::s_EngineExeDirectory;
			NCPath::Join(scriptDllPath, NCPath::CreatePath("ScriptModule.dll"));
			if (File::IsFile(scriptDllPath))
			{
				m_Module = LoadLibraryA(scriptDllPath.Path.c_str());

				if (m_Module)
				{
					m_SaveScripts = (SaveScriptsFn)TryLoadFunction(m_Module, "SaveScripts");
					m_LoadScript = (LoadScriptFn)TryLoadFunction(m_Module, "LoadScript");
					m_UpdateScripts = (UpdateScriptFn)TryLoadFunction(m_Module, "UpdateScripts");
					m_EditorUpdateScripts = (UpdateScriptFn)TryLoadFunction(m_Module, "EditorUpdateScripts");
					m_AddComponentFromString = (AddComponentFn)TryLoadFunction(m_Module, "AddComponent");
					m_InitScripts = (InitScriptsFn)TryLoadFunction(m_Module, "InitScripts");
					m_InitImGui = (InitImGuiFn)TryLoadFunction(m_Module, "InitImGui");
					m_ImGui = (ImGuiFn)TryLoadFunction(m_Module, "ImGui");
					m_IsLoaded = true;

					if (m_InitScripts)
					{
						m_InitScripts(&scene);
					}
				}
			}
		}

		bool FreeScriptLibrary(SceneData& scene)
		{
			if (!m_IsLoaded)
			{
				return true;
			}

			m_SaveScripts = SaveScriptsStub;
			m_LoadScript = LoadScriptStub;
			m_UpdateScripts = UpdateScriptStub;
			m_EditorUpdateScripts = EditorUpdateScriptStub;
			m_AddComponentFromString = AddComponentStub;
			m_InitScripts = InitScriptsStub;
			m_InitImGui = InitImGuiStub;
			m_ImGui = ImGuiStub;

			if (!FreeLibrary(m_Module))
			{
				DWORD errorCode = GetLastError();
				Log::Warning("Could not free script dll. Error Code: %d", errorCode);
				return false;
			}

			m_Module = nullptr;
			m_IsLoaded = false;
			return true;
		}

		void ImGui(SceneData& scene, Entity entity)
		{
			if (m_ImGui)
			{
				m_ImGui(scene.Registry, entity);
			}
		}

		void InitImGui(void* context)
		{
			if (m_InitImGui)
			{
				m_InitImGui(context);
			}
		}

		void SaveScripts(SceneData& scene, json& j)
		{
			if (m_SaveScripts)
			{
				Log::Info("Saving scripts!");
				m_SaveScripts(scene.Registry, j, &scene);
			}
		}

		void AddComponentFromString(std::string className, entt::entity entity, entt::registry& registry)
		{
			if (m_AddComponentFromString)
			{
				m_AddComponentFromString(registry, className, entity);
			}
		}

		void Deserialize(SceneData& scene, json& j, Entity entity)
		{
			if (m_LoadScript)
			{
				m_LoadScript(scene.Registry, j, entity);
			}
		}
	}
}