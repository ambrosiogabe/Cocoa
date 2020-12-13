using JadeNativeScriptProcessor;
using JadeNativeScriptProcessor.src;
using Microsoft.Build.Evaluation;
using Microsoft.Build.Execution;
using Microsoft.Build.Framework;
using Microsoft.Build.Logging;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Threading;
using System.Threading.Tasks;

namespace Reflection
{
	class Program
	{
		static string WatchDirectory;
		static string PremakeExe;
		static string ProjectPremakeLua;
		static List<UClass> Classes = new List<UClass>();

		static void ProcessFile(string file, string generatedDirPath)
		{
			Console.WriteLine($"Processing file '{file}'");
			Scanner fileScanner = new Scanner(file);
			List<Token> fileTokens = fileScanner.ScanTokens();
			Parser fileParser = new Parser(fileTokens, Path.GetFileNameWithoutExtension(file), file);
			fileParser.Parse();
			Classes.AddRange(fileParser.GetClasses());

			if (fileParser.CanGenerateHeaderFile())
			{
				File.WriteAllText(Path.Combine(generatedDirPath, Path.GetFileNameWithoutExtension(file) + "-generated.h"), fileParser.GenerateHeaderFile());
			}
		}

		static bool IsHeaderFile(string extension)
		{
			return extension.Equals(".h") || extension.Equals(".hpp");
		}

		static void Preprocess(string directoryPath)
		{
			if (directoryPath.Contains("generated"))
			{
				// Return if we are in /generated path
				return;
			}

			string generatedDirPath = Path.Combine(directoryPath, "generated");
			foreach (string file in Directory.GetFiles(directoryPath))
			{
				if (IsHeaderFile(Path.GetExtension(file)) && !Path.GetFileName(file).Equals("Reflection.h"))
				{
					if (!Directory.Exists(generatedDirPath))
					{
						Directory.CreateDirectory(generatedDirPath);
					}

					ProcessFile(file, generatedDirPath);
				}
			}

			foreach (string directory in Directory.GetDirectories(directoryPath))
			{
				if (!Path.GetFileName(directory).Equals("generated"))
				{
					Preprocess(directory);
				}
			}
		}

		static void StartFileWatcher()
		{
			string dir = WatchDirectory;
			using (FileSystemWatcher headerWatcher = new FileSystemWatcher())
			using (FileSystemWatcher sourceWatcher = new FileSystemWatcher())
			{
				sourceWatcher.Path = dir;
				sourceWatcher.NotifyFilter = NotifyFilters.LastAccess
					| NotifyFilters.LastWrite
					| NotifyFilters.FileName
					| NotifyFilters.DirectoryName;

				sourceWatcher.Filter = "*.h";
				sourceWatcher.IncludeSubdirectories = true;

				sourceWatcher.Changed += FileChanged;
				sourceWatcher.Created += FileChanged;
				sourceWatcher.Renamed += FileChanged;

				sourceWatcher.EnableRaisingEvents = true;

				headerWatcher.Path = dir;
				headerWatcher.NotifyFilter = NotifyFilters.LastAccess
					| NotifyFilters.LastWrite
					| NotifyFilters.FileName
					| NotifyFilters.DirectoryName;

				headerWatcher.Filter = "*.cpp";
				headerWatcher.IncludeSubdirectories = true;

				headerWatcher.Changed += FileChanged;
				headerWatcher.Created += FileChanged;
				headerWatcher.Renamed += FileChanged;

				headerWatcher.EnableRaisingEvents = true;

				int second = 1000;
				int minute = 60 * second;
				int hour = 60 * minute;
				Task.Run(async () =>
				{
					while (true)
					{
						Console.WriteLine($"C++ preprocessor running at {DateTime.Now}");
						await Task.Delay(hour);
					}
				});

				Console.ReadLine();
			}
		}

		static void RunPremake()
		{
			ProcessStartInfo startInfo = new ProcessStartInfo();
			startInfo.UseShellExecute = false;
			startInfo.RedirectStandardOutput = false;
			startInfo.CreateNoWindow = true;
			startInfo.FileName = PremakeExe;
			startInfo.Arguments = "vs2019 --file=" + ProjectPremakeLua;

			try
			{
				using (Process exeProcess = Process.Start(startInfo))
				{
					exeProcess.WaitForExit();
					Console.WriteLine(exeProcess.StandardOutput.ReadToEnd());
				}
			} 
			catch (Exception e)
			{
				Console.WriteLine(e.Message);
			}
		}

		static void Compile()
		{
			string projectFileName = Path.Combine(Path.GetDirectoryName(ProjectPremakeLua), "ScriptingWorkspace.sln");
			ProcessStartInfo startInfo = new ProcessStartInfo();
			startInfo.UseShellExecute = false;
			startInfo.FileName = "cmd";
			startInfo.RedirectStandardOutput = true;
			startInfo.CreateNoWindow = true;
			string arg = @"/C ""C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat"" && msbuild """ + projectFileName + "\"";// /p:Configuration=Release";
			startInfo.Arguments = arg;

			try
			{
				using (Process exeProcess = Process.Start(startInfo))
				{
					exeProcess.WaitForExit();
					Console.WriteLine(exeProcess.StandardOutput.ReadToEnd());
				}
			}
			catch (Exception e)
			{
				Console.WriteLine(e.Message);
			}

			Console.WriteLine("Done Compiling blah.");
		}

		static void FileChanged(object source, FileSystemEventArgs e)
		{
			if (!e.FullPath.Contains("generated"))
			{
				Classes.Clear();
				Preprocess(Path.GetDirectoryName(e.FullPath));
				InitFileGenerator.GenerateInitFile(WatchDirectory, Classes);
				PremakeGenerator.GeneratePremakeFile(ProjectPremakeLua);
				RunPremake();
				Compile();
			}
		}

		static void Main(string[] args)
		{
			if (args.Length == 2)
			{
				if (Directory.Exists(args[0]))
				{
					Console.WriteLine($"Watching directory '{args[0]}'");
					WatchDirectory = args[0];
					PremakeExe = args[1];

					ProjectPremakeLua = WatchDirectory + "/premake5.lua";
					PremakeGenerator.GeneratePremakeFile(ProjectPremakeLua);
					StartFileWatcher();
				}
				else
				{
					Console.WriteLine($"Incorrect '{args[0]}'");
					Console.WriteLine("Correct Usage: Program.exe <PathToDirectory> <PathToPremake>");
				}
			}
			else
			{
				Console.WriteLine("Correct Usage: Program.exe <PathToDirectory> <PathToPremake>");
			}

			Console.WriteLine("Closing the preprocessor watcher.");
		}
	}
}
