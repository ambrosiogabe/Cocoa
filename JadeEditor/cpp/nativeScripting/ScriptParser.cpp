#include "nativeScripting/ScriptParser.h"

#include "jade/util/Log.h"

namespace Jade
{
	std::string ScriptParser::GenerateHeaderFile()
	{
		std::ostringstream file;
		file << "#pragma once\n\n";
		file << "#define ENTT_STANDARD_CPP\n";
		file << "#include <entt/entt.hpp>\n";
		file << "#include <map>\n";
		file << "#include \"../" << m_FullFilepath.GetFilenameWithoutExt() << ".h\"\n\n";
		file << "namespace Jade{\n";
		file << "\tnamespace Reflect" << m_FullFilepath.GetFilenameWithoutExt() << " \n\t{\n";

		// Append ids as entt hash strings
		file << "\t\tstd::vector<entt::id_type> ids = \n\t\t{\n";

		int i = 0;
		for (auto ustruct : m_Structs)
		{
			int j = 0;
			for (auto uvar : ustruct.m_Variables)
			{
				if (i == m_Structs.size() - 1 && j == ustruct.m_Variables.size() - 1)
					file << "\t\t\t\"" << ustruct.m_StructName.c_str() << "::" << uvar.m_Identifier.c_str() << "\"_hs\n";
				else
					file << "\t\t\t\"" << ustruct.m_StructName.c_str() << "::" << uvar.m_Identifier.c_str() << "\"_hs,\n";

				j++;
			}

			i++;
		}

		file << "\t\t};\n\n";

		// Append debug names map
		file << "\t\tstd::map<entt::id_type, const char*> debugNames = \n\t\t{\n";

		int id = 0;
		i = 0;
		for (auto ustruct : m_Structs)
		{
			int j = 0;
			file << "\t\t\t{ entt::type_info<" << ustruct.m_StructName.c_str() << ">().id(), \"{ustruct.StructName}\"},\n";
			for (auto uvar : ustruct.m_Variables)
			{
				if (i == m_Structs.size() - 1 && j == ustruct.m_Variables.size() - 1)
					file << "\t\t\t{ids[" << id << "], \"" << uvar.m_Identifier.c_str() << "\"}\n";
				else
					file << "\t\t\t{ids[" << id << "], \"" << uvar.m_Identifier.c_str() << "\"},\n";
				
				id++;
				j++;
			}

			i++;
		}

		file << "\t\t};\n\n";

		// Create Init function
		file << "\t\tvoid Init()\n";
		file << "\t\t{\n";
		id = 0;
		for(auto ustruct : m_Structs)
		{
			std::string camelCaseStructName = ustruct.m_StructName;
			camelCaseStructName[0] = tolower(camelCaseStructName[0]);
			file << "\t\t\tauto " << camelCaseStructName.c_str() << "Factory = entt::meta<" << ustruct.m_StructName.c_str() << ">()\n";

			for (auto var : ustruct.m_Variables)
			{
				file << "\t\t\t\t.data<&" << ustruct.m_StructName.c_str() << "::" << var.m_Identifier.c_str() << ">(ids[" << id << "])\n";
				id++;
			}
			file << "\t\t\t\t.type();\n\n";
		}

		for (auto uclass : m_Classes)
		{
			std::string camelCaseClassName = uclass.m_ClassName;
			camelCaseClassName[0] = tolower(camelCaseClassName[0]);
			file << "\t\t\tauto " << camelCaseClassName.c_str() << "Factory = entt::meta<" << uclass.m_ClassName.c_str() << ">()\n";

			for (auto var : uclass.m_Variables)
			{
				file << "\t\t\t\t.data<&" << uclass.m_ClassName.c_str() << "::" << var.m_Identifier.c_str() << ">(ids[" << id << "])\n";
				id++;
			}
			file << "\t\t\t\t.type();\n\n";
		}
		file << "\t\t}\n";

		// Tabs function
		file << "\n"
		"		int tabs = 0;\n"
		"		void printTabs()\n"
		"		{\n"
		"			for (int i = 0; i < tabs; i++)\n"
		"			{\n"
		"				printf(\"\\t\");\n"
		"			}\n"
		"		}\n"
		"\n";

		// Debug printAny function
		file << "\n"
		"		void debugPrintAny(entt::meta_any any)\n"
		"		{\n"
		"			auto typeData = entt::resolve_type(any.type().id());\n"
		"		\n"
		"			for (auto data : typeData.data())\n"
		"			{\n"
		"				tabs++;\n"
		"				auto name = debugNames.find(data.id());\n"
		"				auto type = debugNames.find(data.type().id());\n"
		"				if (name == debugNames.end() && type == debugNames.end())\n"
		"					continue;\n"
		"		\n"
		"				if (data.type().is_class())\n"
		"				{\n"
		"					printTabs();\n"
		"					printf(\"%s<%s>\\n\", name->second, type->second);\n"
		"					tabs++;\n"
		"					entt::meta_handle handle = entt::meta_handle(any);\n"
		"					entt::meta_any resolvedData = data.get(handle);\n"
		"					debugPrintAny(resolvedData);\n"
		"					tabs--;\n"
		"				}\n"
		"				else\n"
		"				{\n"
		"					if (data.type().is_floating_point())\n"
		"					{\n"
		"						printTabs();\n"
		"						entt::meta_handle handle = entt::meta_handle(any);\n"
		"						float val = data.get(handle).cast<float>();\n"
		"						printf(\"%s<float>: %2.3f\\n\", name->second, val);\n"
		"					}\n"
		"		 			else if (data.type().is_integral())\n"
		"					{\n"
		"						printTabs();\n"
		"						entt::meta_handle handle = entt::meta_handle(any);\n"
		"						int val = data.get(handle).cast<int>();\n"
		"						printf(\"%s<int>: %d\\n\", name->second, val);\n"
		"					}\n"
		"				}\n"
		"				tabs--;\n"
		"			}\n"
		"		}\n";

		// Begin template print functions
		file << "\t\ttemplate<typename T>\n";
		file << "\t\tvoid debugPrint(const T& obj)\n";
		file << "\t\t{\n";
		file << "\t\t\tentt::meta_any any = entt::meta_any{ obj };\n";
		file << "\t\t\tdebugPrintAny(any);\n";
		file << "\t\t}\n";

		// End namespace
		file << "\t}\n";

		// End namespace
		file << "}\n";

		return file.str();
	}

	void ScriptParser::DebugPrint()
	{
		for (auto structIter = m_Structs.begin(); structIter != m_Structs.end(); structIter++)
		{
			Log::Info("%s {", structIter->m_StructName.c_str());
			for (auto varIter = structIter->m_Variables.begin(); varIter != structIter->m_Variables.end(); varIter++)
			{
				Log::Info("Type<%s> %s", varIter->m_Type.c_str(), varIter->m_Identifier.c_str());
			}
			Log::Info("}");
		}

		for (auto classIter = m_Classes.begin(); classIter != m_Classes.end(); classIter++)
		{
			Log::Info("%s {", classIter->m_ClassName.c_str());
			for (auto varIter = classIter->m_Variables.begin(); varIter != classIter->m_Variables.end(); varIter++)
			{
				Log::Info("Type<%s> %s", varIter->m_Type.c_str(), varIter->m_Identifier.c_str());
			}
			Log::Info("}");
		}
	}

	void ScriptParser::Parse()
	{
		m_CurrentToken = 0;
		m_CurrentIter = m_Tokens.begin();
		do
		{
			if (Match(TokenType::STRUCT_PROP))
			{
				Expect(TokenType::LEFT_PAREN);
				Expect(TokenType::RIGHT_PAREN);
				Expect(TokenType::STRUCT_KW);
				ParseStruct();
			}
			else if (Match(TokenType::CLASS_PROP))
			{
				Expect(TokenType::LEFT_PAREN);
				Expect(TokenType::RIGHT_PAREN);
				Expect(TokenType::CLASS_KW);
				ParseClass();
			}
			else
			{
				m_CurrentToken++;
				m_CurrentIter++;
			}
		} while (m_CurrentToken < m_Tokens.size() && m_CurrentIter->m_Type != TokenType::END_OF_FILE);
	}

	void ScriptParser::ParseClass()
	{
		Token classType = Expect(TokenType::IDENTIFIER);
		Expect(TokenType::LEFT_BRACKET);

		UClass clazz = UClass{ classType.m_Lexeme, m_FullFilepath, std::list<UVariable>() };

		int level = 1;
		while (m_CurrentIter->m_Type != TokenType::END_OF_FILE)
		{
			if (Match(TokenType::LEFT_BRACKET))
			{
				level++;
			}
			else if (Match(TokenType::RIGHT_BRACKET))
			{
				level--;
				if (level <= 1)
				{
					Expect(TokenType::SEMICOLON);
					break;
				}
			}
			else if (Match(TokenType::PROPERTY))
			{
				Expect(TokenType::LEFT_PAREN);
				Match(TokenType::IDENTIFIER); // Consume any EditAnywhere type thing, it doesn't do anything for now...
				Expect(TokenType::RIGHT_PAREN);
				clazz.m_Variables.push_back(ParseVariable());
			}
			else
			{
				m_CurrentToken++;
				m_CurrentIter++;
			}
		}

		m_Classes.push_back(clazz);
	}

	void ScriptParser::ParseStruct()
	{
		Token structType = Expect(TokenType::IDENTIFIER);
		Expect(TokenType::LEFT_BRACKET);

		UStruct structure = UStruct{ structType.m_Lexeme, m_FullFilepath, std::list<UVariable>() };

		int level = 1;
		while (m_CurrentIter->m_Type != TokenType::END_OF_FILE)
		{
			if (Match(TokenType::LEFT_BRACKET))
			{
				level++;
			}
			else if (Match(TokenType::RIGHT_BRACKET))
			{
				level--;
				if (level <= 1)
				{
					Expect(TokenType::SEMICOLON);
					break;
				}
			}
			else if (Match(TokenType::PROPERTY))
			{
				Expect(TokenType::LEFT_PAREN);
				Match(TokenType::IDENTIFIER); // Consume any EditAnywhere type thing, it doesn't do anything for now...
				Expect(TokenType::RIGHT_PAREN);
				structure.m_Variables.push_back(ParseVariable());
			}
			// TODO: This might need another else statement like class parser
		}

		m_Structs.push_back(structure);
	}

	UVariable ScriptParser::ParseVariable()
	{
		std::list<Token> allTokensBeforeSemiColon = std::list<Token>();
		std::list<Token>::iterator current;
		int afterIdentifierIndex = -1;

		do
		{
			current = m_Tokens.begin();
			std::advance(current, m_CurrentToken);
			if (m_CurrentIter->m_Type == TokenType::SEMICOLON && afterIdentifierIndex == -1)
			{
				afterIdentifierIndex = allTokensBeforeSemiColon.size();
			}
			else if (m_CurrentIter->m_Type == TokenType::EQUAL)
			{
				afterIdentifierIndex = allTokensBeforeSemiColon.size();
			}
			allTokensBeforeSemiColon.push_back(*current);
			m_CurrentToken++;
			m_CurrentIter++;
		} while (current->m_Type != TokenType::END_OF_FILE && current->m_Type != TokenType::SEMICOLON);

		Token& variableIdentifier = GenerateErrorToken();
		if (afterIdentifierIndex != -1)
		{
			if (afterIdentifierIndex <= 1)
			{
				Log::Error("Weird equal sign placement.");
				Log::Error("Line: %d, Column: %d", m_CurrentIter->m_Line, m_CurrentIter->m_Column);
			}
			auto it = allTokensBeforeSemiColon.begin();
			std::advance(it, afterIdentifierIndex - 1);
			variableIdentifier = *it;
		}
		else
		{
			Log::Error("Cannot find variable identifier.");
			Log::Error("Line: %d, Column: %d", m_CurrentIter->m_Line, m_CurrentIter->m_Column);
			return UVariable{ "ERROR", variableIdentifier.m_Lexeme };
		}

		Token& typeIdentifier = GenerateErrorToken();
		auto endIter = allTokensBeforeSemiColon.begin();
		std::advance(endIter, afterIdentifierIndex - 1);
		for (auto iter = allTokensBeforeSemiColon.begin(); iter != endIter; iter++)
		{
			if (iter->m_Type == TokenType::IDENTIFIER)
			{
				typeIdentifier = *iter;
				break;
			}
		}

		return UVariable{ typeIdentifier.m_Lexeme, variableIdentifier.m_Lexeme };
	}

	const Token& ScriptParser::Expect(TokenType type)
	{
		if (m_CurrentIter->m_Type != type)
		{
			Log::Error("Error: Expected '%d' but instead got '%d'", type, m_CurrentIter->m_Type);
			Log::Error("Line: %d, Column: %d", m_CurrentIter->m_Line, m_CurrentIter->m_Column);
			return GenerateErrorToken();
		}

		auto tokenToReturn = m_CurrentIter;
		m_CurrentToken++;
		m_CurrentIter++;
		return *tokenToReturn;
	}

	bool ScriptParser::Match(TokenType type)
	{
		if (m_CurrentIter->m_Type == type)
		{
			m_CurrentToken++;
			m_CurrentIter++;
			return true;
		}

		return false;
	}
}