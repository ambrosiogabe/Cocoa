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
		const JPath& m_FullFilepath;
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
		ScriptParser(const std::list<Token>& tokens, JPath& fullFilepath)
			: m_Tokens(tokens), m_FullFilepath(fullFilepath) {}

		std::string GenerateHeaderFile();
		void DebugPrint();
		void Parse();

		bool CanGenerateHeaderFile() const { return m_Structs.size() != 0 || m_Classes.size() != 0; }
		const std::list<UClass>& GetClasses() { return m_Classes; }

	private:
		void ParseClass();
		void ParseStruct();
		UVariable ParseVariable();
		const Token& Expect(TokenType type);
		bool Match(TokenType type);

	private:
		inline Token GenerateErrorToken() { return Token{ -1, -1, TokenType::ERROR_TYPE, "" }; }

	private:
		std::list<Token>::iterator m_CurrentToken;
		const JPath& m_FullFilepath;

		const std::list<Token>& m_Tokens;
		std::list<UClass> m_Classes;
		std::list<UStruct> m_Structs;
	};
}