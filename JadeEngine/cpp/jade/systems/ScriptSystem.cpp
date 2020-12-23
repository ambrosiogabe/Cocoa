#include "jade/systems/ScriptSystem.h"
#include "jade/file/IFile.h"
#include "jade/util/Settings.h"

namespace Jade
{
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
		if (m_IsLoaded)
		{
			m_DeleteScripts();
			FreeLibrary(m_Module);
		}

		JPath scriptDllPath = Settings::General::s_EngineExeDirectory + JPath("ScriptModule.dll");
		if (IFile::IsFile(scriptDllPath))
		{
			HMODULE module = LoadLibraryA(scriptDllPath.Filepath());

			if (module != nullptr)
			{
				m_SaveScripts = (SaveScriptFn)GetProcAddress(module, "SaveScript");
				m_LoadScripts = (LoadScriptFn)GetProcAddress(module, "LoadScripts");
				m_DeleteScripts = (DeleteScriptFn)GetProcAddress(module, "DeleteScripts");
				m_UpdateScripts = (UpdateScriptFn)GetProcAddress(module, "UpdateScripts");
				m_EditorUpdateScripts = (UpdateScriptFn)GetProcAddress(module, "EditorUpdateScripts");
				m_IsLoaded = true;

				m_LoadScripts();
				Log::Info("LOADED");
			}
		}
	}

	void ScriptSystem::SaveScriptState()
	{
		if (m_IsLoaded)
		{
			m_SaveScripts();
		}
	}
}