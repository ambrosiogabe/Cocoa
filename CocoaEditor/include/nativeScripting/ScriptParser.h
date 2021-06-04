#pragma once
#include "externalLibs.h"
#include "cocoa/core/Core.h"

#include "cocoa/file/CPath.h"

namespace Cocoa
{
	struct UVariable
	{
		std::string m_Type;
		std::string m_Identifier;
		// void* m_Literal;
	};

	struct UClass
	{
		std::string m_ClassName;
		CPath m_FullFilepath;
		std::list<UVariable> m_Variables;
	};

	struct UStruct
	{
		std::string m_StructName;
		const CPath& m_FullFilepath;
		std::list<UVariable> m_Variables;
	};

	class ScriptParser
	{
	public:
		std::string GenerateHeaderFile();
		void DebugPrint();
		void Parse();

		bool CanGenerateHeaderFile() const { return m_Structs.size() != 0 || m_Classes.size() != 0; }
		std::vector<UClass>& GetClasses() { return m_Classes; }

		static std::string GetFilenameAsClassName(std::string filename);

	private:
		int m_CurrentToken;
		const CPath& m_FullFilepath;

		std::vector<UClass> m_Classes;
		std::vector<UStruct> m_Structs;
	};
}