using Reflection;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace JadeNativeScriptProcessor
{
	public class InitFileGenerator
	{

		public static void GenerateInitFile(string watchDirectory, List<UClass> classes)
		{
			Directory.CreateDirectory(Path.Combine(watchDirectory, "scripts", "generated"));
			string InitFilePath = Path.Combine(watchDirectory, "scripts", "generated", "init.h");

			string includes = "";
			foreach (UClass clazz in classes)
			{

			}

			string updateScriptLoop = "\t\textern \"C\" JADE_SCRIPT void UpdateScripts(float dt, Scene* scene)\n\t\t{\n";
			foreach (UClass clazz in classes)
			{
				updateScriptLoop += "\t\t\t{\n";
				updateScriptLoop += $"\t\t\t\tauto view = scene->GetRegistry().view<{clazz.ClassName}>();\n";
				updateScriptLoop += "\t\t\t\tfor (auto entity : view)\n";
				updateScriptLoop += "\t\t\t\t{\n";
				updateScriptLoop += $"\t\t\t\t\tentity.GetComponent<{clazz.ClassName}>().Update(dt);\n";
				updateScriptLoop += "\t\t\t\t}\n";
				updateScriptLoop += "\t\t\t}\n";
			}
			updateScriptLoop += "\t\t}\n";

			string fileContents = @"
#include ""jade/core/Core.h""
#include ""jade/util/Log.h""
#include ""jade/core/Entity.h""

extern ""C"" namespace Jade
{
	extern ""C"" namespace Init
	{
" + updateScriptLoop + @"

		extern ""C"" JADE_SCRIPT void SaveScript(const Script& script, Entity entity)
		{
			Log::Info(""Saving scripts!"");
		}

		extern ""C"" JADE_SCRIPT void LoadScripts()
		{
			Log::Info(""Loading scripts!"");
		}

		extern ""C"" JADE_SCRIPT void DeleteScripts()
		{
			Log::Info(""Deleting scripts!"");
		}
	}
}
".Replace("\r\n", "\n");

			File.WriteAllText(InitFilePath, fileContents);
		}
	}
}
