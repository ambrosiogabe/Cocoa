#pragma once
#include "externalLibs.h"
#include "jade/core/Core.h"

#include "jade/file/JPath.h"

#include "ScriptScanner.h"

namespace Jade
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
		JPath m_FullFilepath;
		std::list<UVariable> m_Variables;
	};

	struct UStruct
	{
		std::string m_StructName;
		const JPath& m_FullFilepath;
		std::list<UVariable> m_Variables;
	};

	class ScriptParser
	{
	public:
		ScriptParser(std::vector<Token>& tokens, JPath& fullFilepath)
			: m_Tokens(tokens), m_FullFilepath(fullFilepath) {}

		std::string GenerateHeaderFile();
		void DebugPrint();
		void Parse();

		bool CanGenerateHeaderFile() const { return m_Structs.size() != 0 || m_Classes.size() != 0; }
		const std::vector<UClass>& GetClasses() { return m_Classes; }

	private:
		void ParseClass();
		void ParseStruct();
		UVariable ParseVariable();
		const Token& Expect(TokenType type);
		bool Match(TokenType type);

	private:
		inline Token GenerateErrorToken() { return Token{ -1, -1, TokenType::ERROR_TYPE, "" }; }

	private:
		int m_CurrentToken;
		std::vector<Token>::iterator m_CurrentIter;
		const JPath& m_FullFilepath;

		std::vector<Token>& m_Tokens;
		std::vector<UClass> m_Classes;
		std::vector<UStruct> m_Structs;
	};
}