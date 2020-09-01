using Microsoft.CSharp;
using System;
using System.IO;
using System.CodeDom.Compiler;

namespace JadeScriptRuntime
{
	class Compiler
	{
		static string sourceCode = "using System;" +
			"namespace Hello " +
			"{" +
				"class HelloWorld" +
				"{" +
					"static void Main(string[] args)" +
					"{" +
						"Console.WriteLine(\"Hello World!\");" +
					"}" +
				"}" +
			"}";

		static void Main(string[] args)
		{
			var codeProvider = new CSharpCodeProvider();

			var parameters = new CompilerParameters();
			parameters.GenerateExecutable = true;

			var results = codeProvider.CompileAssemblyFromSource(parameters, sourceCode);

			if (results.Errors.Count > 0)
			{
				foreach (CompilerError CompError in results.Errors)
				{
					Console.WriteLine($"Line Number: {CompError.Line}, Error Number: {CompError.ErrorNumber}, '{CompError.ErrorText}'");
				}
			}

			Console.WriteLine($"{results.PathToAssembly}");
			File.Copy(results.PathToAssembly, $"{Directory.GetCurrentDirectory()}/Hello.exe");

			Console.ReadLine();
		}
	}
}
