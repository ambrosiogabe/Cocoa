#include "nativeScripting/ScriptParser.h"

#include "cocoa/util/Log.h"

namespace Cocoa
{
	static bool IsAlpha(char c)
	{
		return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
	}
	static bool IsAlphaNumeric(char c)
	{
		return IsAlpha(c) || (c >= '0' && c <= '9');
	}

	static bool IsVariableFriendly(char c)
	{
		return IsAlphaNumeric(c) || c == '_';
	}

	std::string ScriptParser::GetFilenameAsClassName(std::string filename)
	{
		for (int i = 0; i < filename.length(); i++)
		{
			if (i == 0 && !IsAlpha(filename[i]))
			{
				filename[i] = '_';
			}
			else if (!IsVariableFriendly(filename[i]))
			{
				filename[i] = '_';
			}
		}

		return filename;
	}

	std::string ScriptParser::GenerateHeaderFile()
	{
		std::ostringstream file;
		file << "#pragma once\n\n";
		file << "#define ENTT_STANDARD_CPP\n";
		file << "#include <entt/entt.hpp>\n";
		file << "#include <nlohmann/json.hpp>\n";
		file << "#include <imgui.h>\n";
		file << "#include <map>\n\n";
		file << "#include \"ImGuiExtended.h\"\n";
		file << "#include \"../" << NCPath::GetFilenameWithoutExt(m_FullFilepath) << ".h\"\n\n";

		file << "namespace Cocoa\n{\n";
		file << "\tnamespace Reflect" << GetFilenameAsClassName(NCPath::GetFilenameWithoutExt(m_FullFilepath)) << " \n\t{\n";

		file << "\t\tbool initialized = false;\n\n";

		// Append ids as entt hash strings
		file << "\t\tstd::vector<entt::id_type> ids = \n\t\t{\n";

		int i = 0;
		for (auto ustruct : m_Structs)
		{
			int j = 0;
			for (auto uvar : ustruct.m_Variables)
			{
				if (i == m_Structs.size() - 1 && j == ustruct.m_Variables.size() - 1 && m_Classes.size() == 0)
					file << "\t\t\t\"" << ustruct.m_StructName.c_str() << "::" << uvar.m_Identifier.c_str() << "\"_hs\n";
				else
					file << "\t\t\t\"" << ustruct.m_StructName.c_str() << "::" << uvar.m_Identifier.c_str() << "\"_hs,\n";

				j++;
			}

			i++;
		}

		i = 0;
		for (auto uclass : m_Classes)
		{
			int j = 0;
			for (auto uvar : uclass.m_Variables)
			{
				if (i == m_Classes.size() - 1 && j == uclass.m_Variables.size() - 1)
					file << "\t\t\t\"" << uclass.m_ClassName.c_str() << "::" << uvar.m_Identifier.c_str() << "\"_hs\n";
				else
					file << "\t\t\t\"" << uclass.m_ClassName.c_str() << "::" << uvar.m_Identifier.c_str() << "\"_hs,\n";

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
			file << "\t\t\t{ entt::type_info<" << ustruct.m_StructName.c_str() << ">().id(), \"" << ustruct.m_StructName.c_str() << "\"},\n";
			for (auto uvar : ustruct.m_Variables)
			{
				if (i == m_Structs.size() - 1 && j == ustruct.m_Variables.size() - 1 && m_Classes.size() == 0)
					file << "\t\t\t{ids[" << id << "], \"" << uvar.m_Identifier.c_str() << "\"}\n";
				else 
					file << "\t\t\t{ids[" << id << "], \"" << uvar.m_Identifier.c_str() << "\"},\n";
				id++;
				j++;
			}

			i++;
		}

		i = 0;
		for (auto uclass : m_Classes)
		{
			int j = 0;
			file << "\t\t\t{ entt::type_info<" << uclass.m_ClassName.c_str() << ">().id(), \"" << uclass.m_ClassName.c_str() << "\"},\n";
			for (auto uvar : uclass.m_Variables)
			{
				if (i == m_Classes.size() - 1 && j == uclass.m_Variables.size() - 1)
					file << "\t\t\t{ids[" << id << "], \"" << uvar.m_Identifier.c_str() << "\"}\n";
				else
					file << "\t\t\t{ids[" << id << "], \"" << uvar.m_Identifier.c_str() << "\"},\n";

				id++;
				j++;
			}

			i++;
		}

		file << "\t\t};\n\n";

		// Append stringToTypeMap
		file << "\t\tstd::map<std::string, entt::id_type> stringToMap = \n\t\t{\n";

		i = 0;
		for (auto ustruct : m_Structs)
		{
			int j = 0;
			if (i == m_Structs.size() - 1 && m_Classes.size() == 0)
				file << "\t\t\t{ \"" << ustruct.m_StructName.c_str() << "\", entt::type_info<" << ustruct.m_StructName.c_str() << ">().id() }\n";
			else 
				file << "\t\t\t{ \"" << ustruct.m_StructName.c_str() << "\", entt::type_info<" << ustruct.m_StructName.c_str() << ">().id() },\n";
			// TODO: Create variable string to type id mapper...
			i++;
		}

		i = 0;
		for (auto uclass : m_Classes)
		{
			if (i == m_Classes.size() - 1)
				file << "\t\t\t{ \"" << uclass.m_ClassName.c_str() << "\", entt::type_info<" << uclass.m_ClassName.c_str() << ">().id() }\n";
			else
				file << "\t\t\t{ \"" << uclass.m_ClassName.c_str() << "\", entt::type_info<" << uclass.m_ClassName.c_str() << ">().id() },\n";

			i++;
		}

		file << "\t\t};\n\n";

		// Create Init function
		file << "\t\tvoid Init()\n";
		file << "\t\t{\n";
		file << "\t\t\tif (initialized) return;\n";
		file << "\t\t\tinitialized = true;\n\n";

		id = 0;
		for(auto ustruct : m_Structs)
		{
			std::string camelCaseStructName = ustruct.m_StructName;
			camelCaseStructName[0] = tolower(camelCaseStructName[0]);
			file << "\t\t\tauto " << camelCaseStructName.c_str() << "Factory = entt::meta<" << ustruct.m_StructName.c_str() << ">()\n";

			for (auto var : ustruct.m_Variables)
			{
				file << "\t\t\t\t.data<&" << ustruct.m_StructName.c_str() << "::" << var.m_Identifier.c_str() << ", entt::as_ref_t>(ids[" << id << "])\n";
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
				file << "\t\t\t\t.data<&" << uclass.m_ClassName.c_str() << "::" << var.m_Identifier.c_str() << ", entt::as_ref_t>(ids[" << id << "])\n";
				id++;
			}
			file << "\t\t\t\t.type();\n\n";
		}
		file << "\t\t}\n";

		// Create AddComponent function
		file << "\t\tvoid AddComponent(std::string className, entt::entity entity, entt::registry& registry)\n";
		file << "\t\t{\n";

		i = 0;
		for (auto uclass : m_Classes)
		{
			if (i == 0)
				file << "\t\t\tif";
			else
				file << "\t\t\telse if";
			file << "(className == \"" << uclass.m_ClassName.c_str() << "\")\n";
			file << "\t\t\t{\n";
			file << "\t\t\t\tregistry.emplace<" << uclass.m_ClassName.c_str() << ">(entity);\n";
			file << "\t\t\t}\n";
			i++;
		}
		file << "\t\t}\n";

		// SaveScript function
		file << "\n"
		"		void SaveScript(entt::meta_any any, json& j, Entity entity)\n"
		"		{\n"
		"			auto typeData = entt::resolve_type(any.type().id());\n"
		"			\n"
		"			int size = j[\"Components\"].size();\n"
		"			auto typeName = debugNames.find(any.type().id())->second;\n"
		"			json compJson;\n"
		"			compJson[typeName] = {};\n"
		"			compJson[typeName][\"Entity\"] = entity.GetID();\n"
		"\n"
		"			for (auto data : typeData.data())\n"
		"			{\n"
		"				auto name = debugNames.find(data.id());\n"
		"				auto type = debugNames.find(data.type().id());\n"
		"				if (name == debugNames.end() && type == debugNames.end())\n"
		"					continue;\n"
		"\n"
		"				if (data.type().is_floating_point())\n"
		"				{\n"
		"					entt::meta_handle handle = entt::meta_handle(any);\n"
		"					float val = data.get(handle).cast<float>();\n"
		"					compJson[typeName][name->second] = val;\n"
		"				}\n"
		"				else if (data.type().is_integral())\n"
		"				{\n"
		"					entt::meta_handle handle = entt::meta_handle(any);\n"
		"					int val = data.get(handle).cast<int>();\n"
		"					compJson[typeName][name->second] = val;\n"
		"				}\n"
		"			}\n"
		"\n"
		"			j[\"Components\"][size] = compJson;\n"
		"		}\n"
		"\n";

		// Save Scripts function
		file << "\t\tvoid SaveScripts(json& j, entt::registry& registry)\n";
		file << "\t\t{\n";

		for (auto uclass : m_Classes)
		{
			file << "\t\t\t{\n";

			file << "\t\t\t\tauto view = registry.view<" << uclass.m_ClassName.c_str() << ">();\n";
			file << "\t\t\t\tfor (auto entity : view)\n";
			file << "\t\t\t\t{\n";
			file << "\t\t\t\t\tauto comp = registry.get<" << uclass.m_ClassName.c_str() << ">(entity);\n";
			file << "\t\t\t\t\tentt::meta_any any = { comp };\n";
			file << "\t\t\t\t\tSaveScript(any, j, Entity(entity));\n";
			file << "\t\t\t\t}\n";

			file << "\t\t\t}\n";
		}

		file << "\t\t}\n";

		// Load Script function
		file << "\n"
			"		void LoadScript(entt::meta_any any, entt::meta_handle handle, json& j)\n"
			"		{\n"
			"			auto typeData = entt::resolve_type(any.type().id());\n"
			"			auto typeName = debugNames.find(any.type().id())->second;\n"
			"\n"
			"			for (auto data : typeData.data())\n"
			"			{\n"
			"				auto name = debugNames.find(data.id());\n"
			"				auto type = debugNames.find(data.type().id());\n"
			"				if (name == debugNames.end() && type == debugNames.end())\n"
			"					continue;\n"
			"\n"
			"				if (data.type().is_floating_point())\n"
			"				{\n"
			"					if (j[typeName].contains(name->second))\n"
			"						data.get(handle).cast<float>() = j[typeName][name->second];\n"
			"				}\n"
			"				else if (data.type().is_integral())\n"
			"				{\n"
			"					if (j[typeName].contains(name->second))\n"
			"						data.get(handle).cast<int>() = j[typeName][name->second];\n"
			"				}\n"
			"			}\n"
			"		}\n"
			"\n";

		// Try Load Script function
		file << "\t\tvoid TryLoad(json& j, Entity entity, entt::registry& registry)\n";
		file << "\t\t{\n";
		file << "\t\t\tjson::iterator it = j.begin();\n";
		file << "\t\t\tentt::entity e = entity.GetRawEntity();\n";
		file << "\t\t\tif (!registry.valid(e))\n";
		file << "\t\t\t{\n";
		file << "\t\t\t\te = registry.create(e);\n";
		file << "\t\t\t}\n";
		file << "\n";

		i = 0;
		for (auto uclass : m_Classes)
		{
			if (i == 0)
				file << "\t\t\tif";
			else
				file << "\t\t\telse if";

			file << " (it.key() == \"" << uclass.m_ClassName.c_str() << "\")\n";
			file << "\t\t\t{\n";
			file << "\t\t\t\t" << uclass.m_ClassName.c_str() << "& comp = registry.emplace<" << uclass.m_ClassName.c_str() << ">(e);\n";
			file << "\t\t\t\tLoadScript({ comp }, comp, j);\n";
			file << "\t\t\t}\n";

			i++;
		}

		file << "\t\t}\n";

		// ImGuiAny function
		file << "\n"
			"		void ImGuiAny(entt::meta_any any, entt::meta_handle handle)\n"
			"		{\n"
			"			auto typeData = entt::resolve_type(any.type().id());\n"
			"			auto typeName = debugNames.find(any.type().id())->second;\n"
			"\n"
			"			if (ImGui::CollapsingHeader(typeName))\n"
			"			{\n"
			"				CImGui::BeginCollapsingHeaderGroup();\n"
			"				for (auto data : typeData.data())\n"
			"				{\n"
			"					auto name = debugNames.find(data.id());\n"
			"					auto type = debugNames.find(data.type().id());\n"
			"					if (name == debugNames.end() && type == debugNames.end())\n"
			"						continue;\n"
			"\n"
			"					if (data.type().is_floating_point())\n"
			"					{\n"
			"						float& val = data.get(handle).cast<float>();\n"
			"						CImGui::UndoableDragFloat(name->second, val);\n"
			"					}\n"
			"					else if (data.type().is_integral())\n"
			"					{\n"
			"						int& val = data.get(handle).cast<int>();\n"
			"						CImGui::UndoableDragInt(name->second, val);\n"
			"					}\n"
			"				}\n"
			"				CImGui::EndCollapsingHeaderGroup();\n"
			"			}\n"
			"		}\n"
			"\n";

		// ImGui function
		file << "\t\tvoid ImGui(Entity entity, entt::registry& registry)\n";
		file << "\t\t{\n";
		file << "\t\t\tentt::entity e = entity.GetRawEntity();\n";
		file << "\t\t\tif (!registry.valid(e)) return;\n";

		i = 0;
		for (auto uclass : m_Classes)
		{
			file << "\t\t\tif (registry.has<" << uclass.m_ClassName.c_str() << ">(e))\n";
			file << "\t\t\t{\n";
			file << "\t\t\t\t" << uclass.m_ClassName.c_str() << "& comp = registry.get<" << uclass.m_ClassName.c_str() << ">(e);\n";
			file << "\t\t\t\tImGuiAny({ comp }, comp);\n";
			file << "\t\t\t}\n";

			i++;
		}

		file << "\t\t}\n";

		// Delete Scripts function
		file << "\t\tvoid DeleteScripts(entt::registry& registry)\n";
		file << "\t\t{\n";

		for (auto uclass : m_Classes)
		{
			file << "\t\t\t{\n";

			file << "\t\t\t\tregistry.clear<" << uclass.m_ClassName.c_str() << ">();\n";

			file << "\t\t\t}\n";
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
				Match(TokenType::SEMICOLON);
				Expect(TokenType::STRUCT_KW);
				ParseStruct();
			}
			else if (Match(TokenType::CLASS_PROP))
			{
				Expect(TokenType::LEFT_PAREN);
				Expect(TokenType::RIGHT_PAREN);
				Match(TokenType::SEMICOLON);
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

		if (Match(TokenType::COLON))
		{
			while (!Match(TokenType::LEFT_BRACKET)) 
			{
				m_CurrentToken++;
				m_CurrentIter++;
			}
		}
		else
		{
			Expect(TokenType::LEFT_BRACKET);
		}

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
				if (level <= 0)
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
				Match(TokenType::SEMICOLON); // Consume a semicolon if it is there, this is to help with indentation
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
				Match(TokenType::SEMICOLON); // Consume a semicolon if it is there, this is to help with indentation
				structure.m_Variables.push_back(ParseVariable());
			}
			// TODO: This might need another else statement like class parser
		}

		m_Structs.push_back(structure);
	}

	UVariable ScriptParser::ParseVariable()
	{
		std::vector<Token> allTokensBeforeSemiColon = std::vector<Token>();
		std::vector<Token>::iterator current;
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
			// TODO: Create static error token to return a const ref to instead of returning unsafe reference to local variable
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