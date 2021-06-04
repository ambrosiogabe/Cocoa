#include "nativeScripting/ScriptParser.h"

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
		file << "#include <entt/core/hashed_string.hpp>\n";
		file << "#include <entt/core/type_info.hpp>\n";
		file << "#include <entt/meta/factory.hpp>\n";
		file << "#include <entt/meta/meta.hpp>\n";
		file << "#include <entt/meta/resolve.hpp>\n\n";

		file << "#include <nlohmann/json.hpp>\n";
		file << "#include <imgui.h>\n";
		file << "#include <map>\n\n";
		file << "#include \"ImGuiExtended.h\"\n";
		file << "#include \"../" << m_FullFilepath.GetFilenameWithoutExt() << ".h\"\n\n";

		file << "namespace Cocoa\n{\n";
		file << "\tnamespace Reflect" << GetFilenameAsClassName(m_FullFilepath.GetFilenameWithoutExt()) << " \n\t{\n";

		file << "\t\tusing namespace entt::literals;\n";
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
		file << "\t\tstd::map<entt::id_type, const char*> debugNames;\n";

		// Append stringToTypeMap
		file << "\t\tstd::map<std::string, entt::id_type> stringToMap;\n";

		// Create Init function ---------------------------------------------------------------
		file << "\t\tvoid Init()\n";
		file << "\t\t{\n";
		file << "\t\t\tif (initialized) return;\n";
		file << "\t\t\tinitialized = true;\n\n";

		// Debug Names
		file << "\t\tdebugNames = \n\t\t{ \n";

		int id = 0;
		i = 0;
		for (auto ustruct : m_Structs)
		{
			int j = 0;
			file << "\t\t\t{ entt::type_seq<" << ustruct.m_StructName.c_str() << ">::value(), \"" << ustruct.m_StructName.c_str() << "\"},\n";
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
			file << "\t\t\t{ entt::type_seq<" << uclass.m_ClassName.c_str() << ">::value(), \"" << uclass.m_ClassName.c_str() << "\"},\n";
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

		// String to map
		file << "\t\tstringToMap = \n\t\t{ \n";

		i = 0;
		for (auto ustruct : m_Structs)
		{
			int j = 0;
			if (i == m_Structs.size() - 1 && m_Classes.size() == 0)
				file << "\t\t\t{ \"" << ustruct.m_StructName.c_str() << "\", entt::type_seq<" << ustruct.m_StructName.c_str() << ">::value() }\n";
			else
				file << "\t\t\t{ \"" << ustruct.m_StructName.c_str() << "\", entt::type_seq<" << ustruct.m_StructName.c_str() << ">::value() },\n";
			// TODO: Create variable string to type id mapper...
			i++;
		}

		i = 0;
		for (auto uclass : m_Classes)
		{
			if (i == m_Classes.size() - 1)
				file << "\t\t\t{ \"" << uclass.m_ClassName.c_str() << "\", entt::type_seq<" << uclass.m_ClassName.c_str() << ">::value() }\n";
			else
				file << "\t\t\t{ \"" << uclass.m_ClassName.c_str() << "\", entt::type_seq<" << uclass.m_ClassName.c_str() << ">::value() },\n";

			i++;
		}

		file << "\t\t};\n\n";

		// Meta initialization
		id = 0;
		for (auto ustruct : m_Structs)
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
			file << "\t\t\t\tEntity e = NEntity::CreateEntity(entity);\n";
			file << "\t\t\t\tNEntity::AddComponent<" << uclass.m_ClassName.c_str() << ">(e);\n";
			file << "\t\t\t}\n";
			i++;
		}
		file << "\t\t}\n";

		// SaveScript function
		file << "\n"
			"		void SaveScript(entt::meta_any any, json& j, Entity entity)\n"
			"		{\n"
			"			auto typeData = entt::resolve(any.type().id());\n"
			"			\n"
			"			int size = j[\"Components\"].size();\n"
			"			auto typeName = debugNames.find(any.type().id())->second;\n"
			"			json compJson;\n"
			"			compJson[typeName] = {};\n"
			"			compJson[typeName][\"Entity\"] = NEntity::GetID(entity);\n"
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
		file << "\t\tvoid SaveScripts(json& j, entt::registry& registry, SceneData* sceneData)\n";
		file << "\t\t{\n";

		for (auto uclass : m_Classes)
		{
			file << "\t\t\t{\n";

			file << "\t\t\t\tauto view = registry.view<" << uclass.m_ClassName.c_str() << ">();\n";
			file << "\t\t\t\tfor (auto entity : view)\n";
			file << "\t\t\t\t{\n";
			file << "\t\t\t\t\tauto comp = NEntity::GetComponent<" << uclass.m_ClassName.c_str() << ">(NEntity::CreateEntity(entity));\n";
			file << "\t\t\t\t\tentt::meta_any any = { comp };\n";
			file << "\t\t\t\t\tSaveScript(any, j, Entity{ entity });\n";
			file << "\t\t\t\t}\n";

			file << "\t\t\t}\n";
		}

		file << "\t\t}\n";

		// Load Script function
		file << "\n"
			"		void LoadScript(entt::meta_any any, entt::meta_handle handle, json& j)\n"
			"		{\n"
			"			auto typeData = entt::resolve(any.type().id());\n"
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
			"						data.set<float>(handle, j[typeName][name->second]);\n"
			"				}\n"
			"				else if (data.type().is_integral())\n"
			"				{\n"
			"					if (j[typeName].contains(name->second))\n"
			"						data.set<int>(handle, j[typeName][name->second]);\n"
			"				}\n"
			"			}\n"
			"		}\n"
			"\n";

		// Try Load Script function
		file << "\t\tvoid TryLoad(json& j, Entity entity, entt::registry& registry)\n";
		file << "\t\t{\n";
		file << "\t\t\tjson::iterator it = j.begin();\n";
		file << "\t\t\tentt::entity e = entity.Handle;\n";
		file << "\t\t\tif (!registry.valid(e))\n";
		file << "\t\t\t{\n";
		file << "\t\t\t\tLogger::Error(\"Invalid entity, entity does not exist.\");\n";
		file << "\t\t\t\treturn;\n";
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
			file << "\t\t\t\t" << uclass.m_ClassName.c_str() << "& comp = NEntity::AddComponent<" << uclass.m_ClassName.c_str() << ">(entity);\n";
			file << "\t\t\t\tLoadScript({ comp }, comp, j);\n";
			file << "\t\t\t}\n";

			i++;
		}

		file << "\t\t}\n";

		// ImGuiAny function
		file << "\n"
			"		void ImGuiAny(entt::meta_any any, entt::meta_handle handle)\n"
			"		{\n"
			"			auto typeData = entt::resolve(any.type().id());\n"
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
			"						float* val = (float*)data.get(handle).data();\n"
			"						CImGui::UndoableDragFloat(name->second, *val);\n"
			"					}\n"
			"					else if (data.type().is_integral())\n"
			"					{\n"
			"						int* val = (int*)data.get(handle).data();\n"
			"						CImGui::UndoableDragInt(name->second, *val);\n"
			"					}\n"
			"				}\n"
			"				CImGui::EndCollapsingHeaderGroup();\n"
			"			}\n"
			"		}\n"
			"\n";

		// ImGui function
		file << "\t\tvoid ImGui(Entity entity, entt::registry& registry)\n";
		file << "\t\t{\n";
		file << "\t\t\tentt::entity e = entity.Handle;\n";
		file << "\t\t\tif (!registry.valid(e)) return;\n";

		i = 0;
		for (auto uclass : m_Classes)
		{
			file << "\t\t\tif (NEntity::HasComponent<" << uclass.m_ClassName.c_str() << ">(entity))\n";
			file << "\t\t\t{\n";
			file << "\t\t\t\t" << uclass.m_ClassName.c_str() << "& comp = NEntity::GetComponent<" << uclass.m_ClassName.c_str() << ">(entity);\n";
			file << "\t\t\t\tImGuiAny({ comp }, comp);\n";
			file << "\t\t\t}\n";

			i++;
		}

		file << "\t\t}\n";

		// Delete Scripts function
		file << "\t\tvoid DeleteScripts()\n";
		file << "\t\t{\n";

		for (auto uclass : m_Classes)
		{
			file << "\t\t\t{\n";

			file << "\t\t\t\tNEntity::Clear<" << uclass.m_ClassName.c_str() << ">();\n";

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
			"			auto typeData = entt::resolve(any.type().id());\n"
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
			Logger::Info("%s {", structIter->m_StructName.c_str());
			for (auto varIter = structIter->m_Variables.begin(); varIter != structIter->m_Variables.end(); varIter++)
			{
				Logger::Info("Type<%s> %s", varIter->m_Type.c_str(), varIter->m_Identifier.c_str());
			}
			Logger::Info("}");
		}

		for (auto classIter = m_Classes.begin(); classIter != m_Classes.end(); classIter++)
		{
			Logger::Info("%s {", classIter->m_ClassName.c_str());
			for (auto varIter = classIter->m_Variables.begin(); varIter != classIter->m_Variables.end(); varIter++)
			{
				Logger::Info("Type<%s> %s", varIter->m_Type.c_str(), varIter->m_Identifier.c_str());
			}
			Logger::Info("}");
		}
	}

	void ScriptParser::Parse()
	{

	}
}