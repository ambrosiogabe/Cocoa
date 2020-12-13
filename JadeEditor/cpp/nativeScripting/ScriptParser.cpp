#include "nativeScripting/ScriptParser.h"

#include "jade/util/Log.h"

namespace Jade
{
	std::string ScriptParser::GenerateHeaderFile()
	{
		return "";
	}

	void ScriptParser::DebugPrint()
	{
		for (auto structIter = m_Structs.begin(); structIter != m_Structs.end(); structIter++)
		{
			Log::Info("%s {{", structIter->m_StructName.c_str());
			for (auto varIter = structIter->m_Variables.begin(); varIter != structIter->m_Variables.end(); varIter++)
			{
				Log::Info("Type<%s> %s", varIter->m_Type.c_str(), varIter->m_Identifier.c_str());
			}
			Log::Info("}");
		}

		for (auto classIter = m_Classes.begin(); classIter != m_Classes.end(); classIter++)
		{
			Log::Info("%s {{", classIter->m_ClassName.c_str());
			for (auto varIter = classIter->m_Variables.begin(); varIter != classIter->m_Variables.end(); varIter++)
			{
				Log::Info("Type<%s> %s", varIter->m_Type.c_str(), varIter->m_Identifier.c_str());
			}
			Log::Info("}");
		}
	}

	void ScriptParser::Parse()
	{
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
			}
		} while (m_CurrentToken->m_Type != TokenType::END_OF_FILE && m_CurrentToken != m_Tokens.end());
	}

	void ScriptParser::ParseClass()
	{
		Token classType = Expect(TokenType::IDENTIFIER);
		Expect(TokenType::LEFT_BRACKET);

		UClass clazz = UClass{ classType.m_Lexeme, m_FullFilepath, std::list<UVariable>() };

		int level = 1;
		while (m_CurrentToken->m_Type != TokenType::END_OF_FILE)
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
		while (m_CurrentToken->m_Type != TokenType::END_OF_FILE)
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
		}

		m_Structs.push_back(structure);
	}

	UVariable ScriptParser::ParseVariable()
	{
		std::list<Token> allTokensBeforeSemiColon = std::list<Token>();
		int afterIdentifierIndex = -1;

		do
		{
			if (m_CurrentToken->m_Type == TokenType::SEMICOLON && afterIdentifierIndex == -1)
			{
				afterIdentifierIndex = allTokensBeforeSemiColon.size();
			}
			else if (m_CurrentToken->m_Type == TokenType::EQUAL)
			{
				afterIdentifierIndex = allTokensBeforeSemiColon.size();
			}
			allTokensBeforeSemiColon.push_back(*m_CurrentToken);
			m_CurrentToken++;
		} while (m_CurrentToken->m_Type != TokenType::END_OF_FILE && m_CurrentToken->m_Type != TokenType::SEMICOLON);

		Token& variableIdentifier = GenerateErrorToken();
		if (afterIdentifierIndex != -1)
		{
			if (afterIdentifierIndex <= 1)
			{
				Log::Error("Weird equal sign placement.");
				Log::Error("Line: %d, Column: %d", m_CurrentToken->m_Line, m_CurrentToken->m_Column);
			}
			auto it = allTokensBeforeSemiColon.begin();
			std::advance(it, afterIdentifierIndex - 1);
			variableIdentifier = *it;
		}
		else
		{
			Log::Error("Cannot find variable identifier.");
			Log::Error("Line: %d, Column: %d", m_CurrentToken->m_Line, m_CurrentToken->m_Column);
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
		if (m_CurrentToken->m_Type != type)
		{
			Log::Error("Error: Expected '%d' but instead got '%d'", type, m_CurrentToken->m_Type);
			Log::Error("Line: %d, Column: %d", m_CurrentToken->m_Line, m_CurrentToken->m_Column);
			return GenerateErrorToken();
		}

		auto tokenToReturn = m_CurrentToken;
		m_CurrentToken++;
		return *tokenToReturn;
	}

	bool ScriptParser::Match(TokenType type)
	{
		if (m_CurrentToken->m_Type == type)
		{
			m_CurrentToken++;
			return true;
		}

		return false;
	}
}