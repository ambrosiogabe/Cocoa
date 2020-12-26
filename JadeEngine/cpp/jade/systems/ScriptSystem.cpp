#include "jade/systems/ScriptSystem.h"
#include "jade/file/IFile.h"
#include "jade/util/Settings.h"

namespace Jade
{
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

	void ScriptSystem::Reload()
	{
		FreeScriptLibrary();

		JPath scriptDllPath = Settings::General::s_EngineExeDirectory + JPath("ScriptModule.dll");
		if (IFile::IsFile(scriptDllPath))
		{
			HMODULE module = LoadLibraryA(scriptDllPath.Filepath());

			if (module != nullptr)
			{
				m_SaveScripts = (SaveScriptFn)TryLoadFunction(module, "SaveScript");
				m_LoadScripts = (LoadScriptFn)TryLoadFunction(module, "LoadScripts");
				m_DeleteScripts = (DeleteScriptFn)TryLoadFunction(module, "DeleteScripts");
				m_UpdateScripts = (UpdateScriptFn)TryLoadFunction(module, "UpdateScripts");
				m_EditorUpdateScripts = (UpdateScriptFn)TryLoadFunction(module, "EditorUpdateScripts");
				m_AddComponentFromString = (AddComponentFromStringFn)TryLoadFunction(module, "AddComponent");
				m_IsLoaded = true;

				m_LoadScripts();
				Log::Info("Scripts loaded.");
			}
		}
	}

	void ScriptSystem::FreeScriptLibrary()
	{
		if (m_IsLoaded)
		{
			m_DeleteScripts();
			FreeLibrary(m_Module);
			m_IsLoaded = false;
		}
	}

	void ScriptSystem::SaveScriptState()
	{
		if (m_IsLoaded)
		{
			m_SaveScripts();
		}
	}

	void ScriptSystem::AddComponentFromString(std::string className, entt::entity entity, entt::registry& registry)
	{
		if (m_AddComponentFromString)
		{
			m_AddComponentFromString(className, entity, registry);
		}
	}
}