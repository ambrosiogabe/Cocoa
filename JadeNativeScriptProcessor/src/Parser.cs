using System;
using System.Collections.Generic;
using System.Globalization;
using System.Text;

namespace Reflection
{
	public class Parser
	{
		private List<Token> _tokens;
		private List<UClass> _classes;
		private List<UStruct> _structs;
		private string _fileName;
		private string _fullFilePath;

		private int _currentToken;

		public Parser(List<Token> tokens, string fileName, string fullFilePath)
		{
			_fileName = fileName;
			_tokens = tokens;
			_classes = new List<UClass>();
			_structs = new List<UStruct>();
			_currentToken = 0;
			_fullFilePath = fullFilePath;
		}

		public bool CanGenerateHeaderFile()
		{
			return this._structs.Count != 0 || this._classes.Count != 0;
		}

		public List<UClass> GetClasses()
		{
			return this._classes;
		}

		public string GenerateHeaderFile()
		{
			StringBuilder file = new StringBuilder();

			file.Append($"#pragma once\n\n");
			file.Append("#define ENTT_STANDARD_CPP\n");
			file.Append("#include <entt/entt.hpp>\n");
			file.Append("#include <map>\n");
			file.Append($"#include \"../{_fileName}.h\"\n\n");
			file.Append($"namespace Reflect{_fileName} \n{{\n");

			// Append ids as entt hash strings
			file.Append("\tstd::vector<entt::id_type> ids = \n\t{\n");

			for (int i=0; i < _structs.Count; i++)
			{
				UStruct ustruct = _structs[i];
				for (int j=0; j < ustruct.Variables.Count; j++)
				{
					UVariable uvar = ustruct.Variables[j];
					
					if (i == _structs.Count - 1 && j == ustruct.Variables.Count - 1)
						file.Append($"\t\t\"{ustruct.StructName}::{uvar.Identifier}\"_hs\n");
					else
						file.Append($"\t\t\"{ustruct.StructName}::{uvar.Identifier}\"_hs,\n");
				}
			}

			file.Append("\t};\n\n");

			// Append debug names map
			file.Append("\tstd::map<entt::id_type, const char*> debugNames = \n\t{\n");

			int id = 0;
			for (int i = 0; i < _structs.Count; i++)
			{
				UStruct ustruct = _structs[i];
				file.Append($"\t\t{{ entt::type_info<{ustruct.StructName}>().id(), \"{ustruct.StructName}\"}},\n");
				for (int j = 0; j < ustruct.Variables.Count; j++)
				{
					UVariable uvar = ustruct.Variables[j];

					if (i == _structs.Count - 1 && j == ustruct.Variables.Count - 1)
						file.Append($"\t\t{{ids[{id}], \"{uvar.Identifier}\"}}\n");
					else
						file.Append($"\t\t{{ids[{id}], \"{uvar.Identifier}\"}},\n");
					id++;
				}
			}

			file.Append("\t};\n\n");

			// Create Init function
			file.Append("\tvoid Init()\n");
			file.Append("\t{\n");
			id = 0;
			foreach (UStruct ustruct in _structs)
			{
				string camelCaseStructName = ustruct.StructName.Substring(0, 1).ToLower() + ustruct.StructName.Substring(1, ustruct.StructName.Length - 1);
				file.Append($"\t\tauto {camelCaseStructName}Factory = entt::meta<{ustruct.StructName}>()\n");

				foreach (UVariable var in ustruct.Variables)
				{
					file.Append($"\t\t\t.data<&{ustruct.StructName}::{var.Identifier}>(ids[{id}])\n");
					id++;
				}
				file.Append("\t\t\t.type();\n\n");
			}

			foreach (UClass uclass in _classes)
			{
				string camelCaseStructName = uclass.ClassName.Substring(0, 1).ToLower() + uclass.ClassName.Substring(1, uclass.ClassName.Length - 1);
				file.Append($"\t\tauto {camelCaseStructName}Factory = entt::meta<{uclass.ClassName}>()\n");

				foreach (UVariable var in uclass.Variables)
				{
					file.Append($"\t\t\t.data<&{uclass.ClassName}::{var.Identifier}>(ids[{id}])\n");
					id++;
				}
				file.Append("\t\t\t.type();\n\n");
			}
			file.Append("\t}\n");

			// Tabs function
			file.Append(@"
	int tabs = 0;

	void printTabs()
	{
		for (int i = 0; i < tabs; i++)
		{
			printf(""\t"");
		}
	}

".Replace("\r\n", "\n"));

			// Add debugPrintAny function
			file.Append(@"
	void debugPrintAny(entt::meta_any any)
	{
		auto typeData = entt::resolve_type(any.type().id());

		for (auto data : typeData.data())
		{
			tabs++;
			auto name = debugNames.find(data.id());
			auto type = debugNames.find(data.type().id());
			if (name == debugNames.end() && type == debugNames.end())
				continue;

			if (data.type().is_class())
			{
				printTabs();
				printf(""%s<%s>\n"", name->second, type->second);
				tabs++;
				entt::meta_handle handle = entt::meta_handle(any);
				entt::meta_any resolvedData = data.get(handle);
				debugPrintAny(resolvedData);
				tabs--;
			}
			else
			{
				if (data.type().is_floating_point())
				{
					printTabs();
					entt::meta_handle handle = entt::meta_handle(any);
					float val = data.get(handle).cast<float>();
					printf(""%s<float>: %2.3f\n"", name->second, val);
				}
				else if (data.type().is_integral())
				{
					printTabs();
					entt::meta_handle handle = entt::meta_handle(any);
					int val = data.get(handle).cast<int>();
					printf(""%s<int>: %d\n"", name->second, val);
				}
			}
			tabs--;
		}
	}

".Replace("\r\n", "\n"));

			// Begin template print functions
			file.Append("\ttemplate<typename T>\n");
			file.Append("\tvoid debugPrint(const T& obj)\n");
			file.Append("\t{\n");
			file.Append("\t\tentt::meta_any any = entt::meta_any{ obj };\n");
			file.Append("\t\tdebugPrintAny(any);\n");
			file.Append("\t}\n");

			// End class
			file.Append("};\n");

			return file.ToString();
		}

		public void DebugPrint()
		{
			foreach (var uStruct in _structs)
			{
				Console.WriteLine($"{uStruct.StructName} {{");
				foreach (var uVar in uStruct.Variables)
				{
					Console.WriteLine($"\tType<{uVar.Type}> {uVar.Identifier}");
				}
				Console.WriteLine("}");
			}

			foreach (var uClass in _classes)
			{
				Console.WriteLine($"{uClass.ClassName} {{");
				foreach (var uVar in uClass.Variables)
				{
					Console.WriteLine($"\tType<{uVar.Type}> {uVar.Identifier}");
				}
				Console.WriteLine("}");
			}
		}

		public void Parse()
		{
			Token token;
			do
			{
				token = _tokens[_currentToken];

				if (_Match(TokenType.STRUCT_PROP))
				{
					_Expect(TokenType.LEFT_PAREN);
					_Expect(TokenType.RIGHT_PAREN);
					_Expect(TokenType.STRUCT_KW);
					_ParseStruct();
				} 
				else if (_Match(TokenType.CLASS_PROP))
				{
					_Expect(TokenType.LEFT_PAREN);
					_Expect(TokenType.RIGHT_PAREN);
					_Expect(TokenType.CLASS_KW);
					_ParseClass();
				}
				else
				{
					_currentToken++;
				}
			} while (token.Type != TokenType.EOF && _currentToken < _tokens.Count - 1);
		}

		private void _ParseClass()
		{
			Token classType = _Expect(TokenType.IDENTIFIER);
			_Expect(TokenType.LEFT_BRACKET);

			UClass clazz = new UClass { Variables = new List<UVariable>(), ClassName = classType.Lexeme, FullPathToFile = _fullFilePath };

			int level = 1;
			Token token = _tokens[_currentToken];
			while (token.Type != TokenType.EOF)
			{
				if (_Match(TokenType.LEFT_BRACKET))
				{
					level++;
				}
				else if (_Match(TokenType.RIGHT_BRACKET))
				{
					level--;
					if (level <= 1)
					{
						_Expect(TokenType.SEMICOLON);
						break;
					}
				}
				else if (_Match(TokenType.PROPERTY))
				{
					_Expect(TokenType.LEFT_PAREN);
					_Match(TokenType.IDENTIFIER); // Consume any EditAnywhere type thing, it doesn't do anything for now...
					_Expect(TokenType.RIGHT_PAREN);
					clazz.Variables.Add(_ParseVariable());
				}
				else
				{
					_currentToken++;
				}
			}

			_classes.Add(clazz);
		}

		private void _ParseStruct()
		{
			Token structType = _Expect(TokenType.IDENTIFIER);
			_Expect(TokenType.LEFT_BRACKET);

			UStruct structure = new UStruct { Variables = new List<UVariable>(), StructName = structType.Lexeme };

			int level = 1;
			Token token = _tokens[_currentToken];
			while (token.Type != TokenType.EOF)
			{
				if (_Match(TokenType.LEFT_BRACKET))
				{
					level++;
				} 
				else if (_Match(TokenType.RIGHT_BRACKET))
				{
					level--;
					if (level <= 1)
					{
						_Expect(TokenType.SEMICOLON);
						break;
					}
				} 
				else if (_Match(TokenType.PROPERTY))
				{
					_Expect(TokenType.LEFT_PAREN);
					_Match(TokenType.IDENTIFIER); // Consume any EditAnywhere type thing, it doesn't do anything for now...
					_Expect(TokenType.RIGHT_PAREN);
					structure.Variables.Add(_ParseVariable());
				}
			}

			_structs.Add(structure);
		}

		private UVariable _ParseVariable()
		{
			List<Token> allTokensBeforeSemiColon = new List<Token>();
			Token current;
			int afterIdentifierIndex = -1;

			do
			{
				current = _tokens[_currentToken];
				if (current.Type == TokenType.SEMICOLON && afterIdentifierIndex == -1)
				{
					afterIdentifierIndex = allTokensBeforeSemiColon.Count;
				}
				else if (current.Type == TokenType.EQUAL)
				{
					afterIdentifierIndex = allTokensBeforeSemiColon.Count;
				}
				allTokensBeforeSemiColon.Add(current);
				_currentToken++;
			} while (current.Type != TokenType.EOF && current.Type != TokenType.SEMICOLON);

			Token variableIdentifier = null;
			if (afterIdentifierIndex != -1)
			{
				if (afterIdentifierIndex <= 1)
				{
					Console.WriteLine($"Error: Weird equal sign placement.");
					Console.WriteLine($"\tLine: {_tokens[_currentToken].Line}, Column: {_tokens[_currentToken].Column}");
				}
				variableIdentifier = allTokensBeforeSemiColon[afterIdentifierIndex - 1];
			} 
			else
			{
				Console.WriteLine($"Error: Cannot find variable identifier.");
				Console.WriteLine($"\tLine: {_tokens[_currentToken].Line}, Column: {_tokens[_currentToken].Column}");
			}

			Token typeIdentifier = null;
			for (int i=0; i < afterIdentifierIndex - 1; i++)
			{
				if (allTokensBeforeSemiColon[i].Type == TokenType.IDENTIFIER)
				{
					typeIdentifier = allTokensBeforeSemiColon[i];
					break;
				}
			}

			return new UVariable { Type = typeIdentifier.Lexeme, Identifier = variableIdentifier.Lexeme, Literal = null };
		}

		private Token _Expect(TokenType type)
		{
			if (_tokens[_currentToken].Type != type)
			{
				Console.WriteLine($"Error: Expected '{type.ToString()}' but instead got '{_tokens[_currentToken].Type.ToString()}'");
				Console.WriteLine($"\tLine: {_tokens[_currentToken].Line}, Column: {_tokens[_currentToken].Column}");
				throw new Exception();
			}

			_currentToken++;
			return _tokens[_currentToken - 1];
		}

		private bool _Match(TokenType type)
		{
			if (_tokens[_currentToken].Type == type)
			{
				_currentToken++;
				return true;
			}

			return false;
		}
	}
}
