#include "jade/systems/ScriptSystem.h"
#include "jade/file/IFile.h"
#include "jade/util/Settings.h"

namespace Jade
{
	void ScriptSystem::Update(float dt)
	{
		JPath scriptDllPath = Settings::General::s_EngineExeDirectory + "ScriptModule.dll";
		typedef void(*SaveScriptFn)();
		typedef void(*DeleteScriptFn)();
		typedef void(*LoadScriptFn)();
		if (IFile::IsFile(scriptDllPath))
		{
			HMODULE module = LoadLibraryA(scriptDllPath.Filepath());
			SaveScriptFn saveScriptFn = (SaveScriptFn)GetProcAddress(module, "SaveScripts");
			LoadScriptFn loadScriptFn = (LoadScriptFn)GetProcAddress(module, "LoadScripts");
			DeleteScriptFn deleteScriptFn = (DeleteScriptFn)GetProcAddress(module, "DeleteScripts");

			//saveScriptFn();
			//loadScriptFn();
			//deleteScriptFn();

			FreeLibrary(module);
		}
	}
}