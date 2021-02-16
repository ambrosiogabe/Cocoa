#include "cocoa/systems/ScriptSystem.h"
#include "cocoa/file/File.h"
#include "cocoa/util/Settings.h"

#ifdef _WIN32
#include <Windows.h>
#include <libloaderapi.h>
#endif

namespace Cocoa
{
	namespace ScriptSystem
	{
		// Internal Variables
		static LoadScriptFn m_LoadScript = nullptr;
		static SaveScriptFn m_SaveScripts = nullptr;
		static DeleteScriptFn m_DeleteScripts = nullptr;
		static UpdateScriptFn m_UpdateScripts = nullptr;
		static InitScriptsFn m_InitScripts = nullptr;
		static EditorUpdateScriptFn m_EditorUpdateScripts = nullptr;
		static AddComponentFromStringFn m_AddComponentFromString = nullptr;
		static ImGuiFn m_ImGui = nullptr;
		static InitImGuiFn m_InitImGui = nullptr;
		
		static bool m_IsLoaded = false;
		static HMODULE m_Module;

		// Forward Declarations
		static void DeleteScriptStub() {}
		static void InitScriptsStub() {}
		static void InitImGuiStub(void*) {}
		static void SaveScriptStub(json&) {}
		static void LoadScriptStub(json&, Entity) {}
		static void ImGuiStub(Entity) {}
		static void UpdateScriptStub(float, SceneData*) {}
		static void EditorUpdateScriptStub(float, SceneData*) {}
		static void AddComponentFromStringStub(std::string, entt::entity, entt::registry&) { Log::Warning("Adding component from STUB"); }

		static FARPROC __stdcall TryLoadFunction(HMODULE module, const char* functionName)
		{
			auto func = GetProcAddress(module, functionName);
			if (func == NULL)
			{
				Log::Warning("Could not load dll function '%s'", functionName);
			}

			return func;
		}

		void Init()
		{
			Reload();
		}

		void Update(SceneData& scene, float dt)
		{
			if (m_UpdateScripts)
			{
				m_UpdateScripts(dt, &scene);
			}
		}

		void EditorUpdate(SceneData& scene, float dt)
		{
			if (m_EditorUpdateScripts)
			{
				m_EditorUpdateScripts(dt, &scene);
			}
		}

		void Reload()
		{
			if (m_IsLoaded)
			{
				if (!FreeScriptLibrary()) return;
			}

			CPath scriptDllPath = Settings::General::s_EngineExeDirectory;
			NCPath::Join(scriptDllPath, NCPath::CreatePath("ScriptModule.dll"));
			if (File::IsFile(scriptDllPath))
			{
				m_Module = LoadLibraryA(scriptDllPath.Path.c_str());

				if (m_Module)
				{
					m_SaveScripts = (SaveScriptFn)TryLoadFunction(m_Module, "SaveScripts");
					m_LoadScript = (LoadScriptFn)TryLoadFunction(m_Module, "LoadScript");
					m_DeleteScripts = (DeleteScriptFn)TryLoadFunction(m_Module, "DeleteScripts");
					m_UpdateScripts = (UpdateScriptFn)TryLoadFunction(m_Module, "UpdateScripts");
					m_EditorUpdateScripts = (UpdateScriptFn)TryLoadFunction(m_Module, "EditorUpdateScripts");
					m_AddComponentFromString = (AddComponentFromStringFn)TryLoadFunction(m_Module, "AddComponent");
					m_InitScripts = (InitScriptsFn)TryLoadFunction(m_Module, "InitScripts");
					m_InitImGui = (InitImGuiFn)TryLoadFunction(m_Module, "InitImGui");
					m_ImGui = (ImGuiFn)TryLoadFunction(m_Module, "ImGui");
					m_IsLoaded = true;

					if (m_InitScripts)
					{
						m_InitScripts();
					}
				}
			}
		}

		bool FreeScriptLibrary()
		{
			if (m_DeleteScripts)
			{
				m_DeleteScripts();
			}

			m_SaveScripts = SaveScriptStub;
			m_LoadScript = LoadScriptStub;
			m_DeleteScripts = DeleteScriptStub;
			m_UpdateScripts = UpdateScriptStub;
			m_EditorUpdateScripts = EditorUpdateScriptStub;
			m_AddComponentFromString = AddComponentFromStringStub;
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

		void ImGui(Entity entity)
		{
			if (m_ImGui)
			{
				m_ImGui(entity);
			}
		}

		void InitImGui(void* context)
		{
			if (m_InitImGui)
			{
				m_InitImGui(context);
			}
		}

		void SaveScripts(json& j)
		{
			if (m_SaveScripts)
			{
				Log::Info("Saving scripts!");
				m_SaveScripts(j);
			}
		}

		void AddComponentFromString(std::string className, entt::entity entity, entt::registry& registry)
		{
			if (m_AddComponentFromString)
			{
				m_AddComponentFromString(className, entity, registry);
			}
		}

		void Deserialize(json& j, Entity entity)
		{
			if (m_LoadScript)
			{
				m_LoadScript(j, entity);
			}
		}
	}
}