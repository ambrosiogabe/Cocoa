#include "cocoa/systems/ScriptSystem.h"
#include "cocoa/file/IFile.h"
#include "cocoa/util/Settings.h"

namespace Cocoa
{
	static void DeleteScriptStub() {}
	static void InitScriptsStub() {}
	static void InitImGuiStub(ImGuiContext*) {}
	static void SaveScriptStub(json&) {}
	static void LoadScriptStub(json&, Entity) {}
	static void ImGuiStub(Entity) {}
	static void UpdateScriptStub(float, Scene*) {}
	static void EditorUpdateScriptStub(float, Scene*) { Log::Info("STUB UPDATE;"); }
	static void AddComponentFromStringStub(std::string, entt::entity, entt::registry&) {}

	static FARPROC __stdcall TryLoadFunction(HMODULE module, const char* functionName)
	{
		auto func = GetProcAddress(module, functionName);
		if (func == NULL)
		{
			Log::Warning("Could not load dll function '%s'", functionName);
		}
		
		return func;
	}

	void ScriptSystem::Start()
	{
		Reload();
	}

	void ScriptSystem::Update(float dt)
	{
		if (m_UpdateScripts)
		{
			m_UpdateScripts(dt, this->m_Scene);
		}
	}

	void ScriptSystem::EditorUpdate(float dt)
	{
		if (m_EditorUpdateScripts)
		{
			m_EditorUpdateScripts(dt, this->m_Scene);
		}
	}

	void ScriptSystem::Reload()
	{
		if (m_IsLoaded)
		{
			if (!FreeScriptLibrary()) return;
		}

		CPath scriptDllPath = Settings::General::s_EngineExeDirectory + CPath("ScriptModule.dll");
		if (IFile::IsFile(scriptDllPath))
		{
			m_Module = LoadLibraryA(scriptDllPath.Filepath());

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

	bool ScriptSystem::FreeScriptLibrary()
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

	void ScriptSystem::ImGui(Entity entity)
	{
		if (m_ImGui)
		{
			m_ImGui(entity);
		}
	}

	void ScriptSystem::SaveScripts(json& j)
	{
		if (m_SaveScripts)
		{
			Log::Info("Saving scripts!");
			m_SaveScripts(j);
		}
	}

	void ScriptSystem::AddComponentFromString(std::string className, entt::entity entity, entt::registry& registry)
	{
		if (m_AddComponentFromString)
		{
			m_AddComponentFromString(className, entity, registry);
		}
	}

	void ScriptSystem::Deserialize(json& j, Entity entity)
	{
		if (m_LoadScript)
		{
			m_LoadScript(j, entity);
		}
	}

	ScriptSystem::~ScriptSystem()
	{
		FreeScriptLibrary();
	}
}