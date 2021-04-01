#include "cocoaTests/include/ScriptParserTests.h"

#include "nativeScripting/ScriptParser.h"
#include "nativeScripting/ScriptScanner.h"

#include "cocoa/file/CPath.h"
#include "cocoa/util/Log.h"
#include "cocoa/util/Settings.h"

namespace Cocoa
{
	namespace ScriptParserTests
	{
		void TestTokenScanning()
		{
			CPath testFile = Settings::General::s_EngineSourceDirectory;
			NCPath::Join(testFile, NCPath::CreatePath("CocoaEditor/tests/cocoaTests/src/ScanningTestFile.cpp"));
			ScriptScanner scanner = ScriptScanner(testFile);
			std::vector<Token> tokens = scanner.ScanTokens();

			int counter = 0;
			// Line 1
			Log::Assert(tokens[counter++].m_Type == TokenType::MACRO_IF, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::KW_FALSE, "TestTokenScanning");

			// Lines 11-17 of TestFile.cpp
			Log::Assert(tokens[counter++].m_Type == TokenType::KW_NAMESPACE, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::IDENTIFIER, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::LEFT_CURLY_BRACKET, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::KW_NAMESPACE, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::IDENTIFIER, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::LEFT_CURLY_BRACKET, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::RIGHT_CURLY_BRACKET, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::RIGHT_CURLY_BRACKET, "TestTokenScanning");

			// Lines 19-31 TestAngleBracketTokens()
			// template<typename Foo>
			Log::Assert(tokens[counter++].m_Type == TokenType::KW_TEMPLATE, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::LEFT_ANGLE_BRACKET, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::KW_TYPENAME, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::IDENTIFIER, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::RIGHT_ANGLE_BRACKET, "TestTokenScanning");
			// Function declaration
			Log::Assert(tokens[counter++].m_Type == TokenType::KW_VOID, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::IDENTIFIER, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::LEFT_PAREN, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::RIGHT_PAREN, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::LEFT_CURLY_BRACKET, "TestTokenScanning");
			// Line 22
			Log::Assert(tokens[counter++].m_Type == TokenType::KW_INT, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::IDENTIFIER, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::EQUAL, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::INTEGER_LITERAL, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::LEFT_SHIFT, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::INTEGER_LITERAL, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::SEMICOLON, "TestTokenScanning");
			// Line 23
			Log::Assert(tokens[counter++].m_Type == TokenType::IDENTIFIER, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::LEFT_SHIFT_EQUAL, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::INTEGER_LITERAL, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::SEMICOLON, "TestTokenScanning");
			// Line 24
			Log::Assert(tokens[counter++].m_Type == TokenType::IDENTIFIER, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::RIGHT_SHIFT_EQUAL, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::INTEGER_LITERAL, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::SEMICOLON, "TestTokenScanning");
			// Line 25
			Log::Assert(tokens[counter++].m_Type == TokenType::IDENTIFIER, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::EQUAL, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::INTEGER_LITERAL, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::RIGHT_SHIFT, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::INTEGER_LITERAL, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::SEMICOLON, "TestTokenScanning");
			// Line 26
			Log::Assert(tokens[counter++].m_Type == TokenType::KW_INT, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::IDENTIFIER, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::EQUAL, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::INTEGER_LITERAL, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::SEMICOLON, "TestTokenScanning");
			// Line 27
			Log::Assert(tokens[counter++].m_Type == TokenType::IDENTIFIER, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::LEFT_ANGLE_BRACKET, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::IDENTIFIER, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::SEMICOLON, "TestTokenScanning");
			// Line 28
			Log::Assert(tokens[counter++].m_Type == TokenType::IDENTIFIER, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::RIGHT_ANGLE_BRACKET, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::IDENTIFIER, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::SEMICOLON, "TestTokenScanning");
			// Line 29
			Log::Assert(tokens[counter++].m_Type == TokenType::IDENTIFIER, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::LESS_THAN_EQ, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::IDENTIFIER, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::SEMICOLON, "TestTokenScanning");
			// Line 30
			Log::Assert(tokens[counter++].m_Type == TokenType::IDENTIFIER, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::GREATER_THAN_EQ, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::IDENTIFIER, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::SEMICOLON, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::RIGHT_CURLY_BRACKET, "TestTokenScanning");

			// Lines 33-88 TestDecimalNumberLiterals()
			// Function Declaration
			Log::Assert(tokens[counter++].m_Type == TokenType::KW_VOID, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::IDENTIFIER, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::LEFT_PAREN, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::RIGHT_PAREN, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::LEFT_CURLY_BRACKET, "TestTokenScanning");
			// Lines 36-43
			for (int i = 0; i < 7; i++)
			{
				Log::Assert(tokens[counter++].m_Type == TokenType::KW_INT, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::IDENTIFIER, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::EQUAL, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::INTEGER_LITERAL, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::SEMICOLON, "TestTokenScanning");
			}
			// Lines 52-55
			for (int i = 0; i < 4; i++)
			{
				Log::Assert(tokens[counter++].m_Type == TokenType::KW_UNSIGNED, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::IDENTIFIER, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::EQUAL, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::INTEGER_LITERAL, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::SEMICOLON, "TestTokenScanning");
			}
			// Lines 59-62
			for (int i = 0; i < 4; i++)
			{
				Log::Assert(tokens[counter++].m_Type == TokenType::KW_LONG, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::IDENTIFIER, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::EQUAL, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::INTEGER_LITERAL, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::SEMICOLON, "TestTokenScanning");
			}
			// Lines 66-71
			for (int i = 0; i < 6; i++)
			{
				Log::Assert(tokens[counter++].m_Type == TokenType::KW_UNSIGNED, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::KW_LONG, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::IDENTIFIER, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::EQUAL, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::INTEGER_LITERAL, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::SEMICOLON, "TestTokenScanning");
			}
			// Lines 75-80
			for (int i = 0; i < 6; i++)
			{
				Log::Assert(tokens[counter++].m_Type == TokenType::KW_LONG, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::KW_LONG, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::IDENTIFIER, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::EQUAL, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::INTEGER_LITERAL, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::SEMICOLON, "TestTokenScanning");
			}
			// Lines 85-86
			for (int i = 0; i < 2; i++)
			{
				Log::Assert(tokens[counter++].m_Type == TokenType::KW_UNSIGNED, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::KW_LONG, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::KW_LONG, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::IDENTIFIER, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::EQUAL, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::INTEGER_LITERAL, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::SEMICOLON, "TestTokenScanning");
			}
			// Line 87
			Log::Assert(tokens[counter++].m_Type == TokenType::KW_AUTO, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::IDENTIFIER, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::EQUAL, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::INTEGER_LITERAL, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::SEMICOLON, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::RIGHT_CURLY_BRACKET, "TestTokenScanning");

			// Lines 90-113 TestFloatingPointNumberLiterals()
			// Function Declaration
			Log::Assert(tokens[counter++].m_Type == TokenType::KW_VOID, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::IDENTIFIER, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::LEFT_PAREN, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::RIGHT_PAREN, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::LEFT_CURLY_BRACKET, "TestTokenScanning");
			// Lines 94-105
			for (int i = 0; i < 7; i++)
			{
				Log::Assert(tokens[counter++].m_Type == TokenType::KW_DOUBLE, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::IDENTIFIER, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::EQUAL, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::FLOATING_POINT_LITERAL, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::SEMICOLON, "TestTokenScanning");
			}
			// Lines 106-112
			for (int i = 0; i < 7; i++)
			{
				Log::Assert(tokens[counter++].m_Type == TokenType::KW_FLOAT, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::IDENTIFIER, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::EQUAL, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::FLOATING_POINT_LITERAL, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::SEMICOLON, "TestTokenScanning");
			}
			Log::Assert(tokens[counter++].m_Type == TokenType::RIGHT_CURLY_BRACKET, "TestTokenScanning");

			// Lines 115-120 TestBinaryLiterals()
			// Function Declaration
			Log::Assert(tokens[counter++].m_Type == TokenType::KW_VOID, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::IDENTIFIER, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::LEFT_PAREN, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::RIGHT_PAREN, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::LEFT_CURLY_BRACKET, "TestTokenScanning");
			// Lines 118-119
			for (int i = 0; i < 2; i++)
			{
				Log::Assert(tokens[counter++].m_Type == TokenType::KW_INT, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::IDENTIFIER, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::EQUAL, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::INTEGER_LITERAL, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::SEMICOLON, "TestTokenScanning");
			}
			Log::Assert(tokens[counter++].m_Type == TokenType::RIGHT_CURLY_BRACKET, "TestTokenScanning");

			// Lines 122-150 TestCharacterLiterals()
			// Function Declaration
			Log::Assert(tokens[counter++].m_Type == TokenType::KW_VOID, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::IDENTIFIER, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::LEFT_PAREN, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::RIGHT_PAREN, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::LEFT_CURLY_BRACKET, "TestTokenScanning");
			// Lines 125-149
			for (int i = 0; i < 21; i++)
			{
				Log::Assert(tokens[counter++].m_Type == TokenType::KW_AUTO, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::IDENTIFIER, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::EQUAL, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::CHARACTER_LITERAL, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::SEMICOLON, "TestTokenScanning");
			}
			Log::Assert(tokens[counter++].m_Type == TokenType::RIGHT_CURLY_BRACKET, "TestTokenScanning");

			// Lines 152-182 TestStringLiterals()
			// Function Declaration
			Log::Assert(tokens[counter++].m_Type == TokenType::KW_VOID, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::IDENTIFIER, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::LEFT_PAREN, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::RIGHT_PAREN, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::LEFT_CURLY_BRACKET, "TestTokenScanning");
			// Lines 155-166
			for (int i = 0; i < 10; i++)
			{
				Log::Assert(tokens[counter++].m_Type == TokenType::KW_AUTO, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::IDENTIFIER, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::EQUAL, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::STRING_LITERAL, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::SEMICOLON, "TestTokenScanning");
			}
			// Lines 168-169
			Log::Assert(tokens[counter++].m_Type == TokenType::KW_CONST, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::KW_CHAR, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::STAR, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::IDENTIFIER, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::EQUAL, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::STRING_LITERAL, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::SEMICOLON, "TestTokenScanning");
			// Line 170
			Log::Assert(tokens[counter++].m_Type == TokenType::KW_CONST, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::KW_CHAR, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::STAR, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::IDENTIFIER, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::EQUAL, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::STRING_LITERAL, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::STRING_LITERAL, "TestTokenScanning");
			Log::Assert(tokens[counter++].m_Type == TokenType::SEMICOLON, "TestTokenScanning");
			// Lines 172-181
			for (int i = 0; i < 2; i++)
			{
				Log::Assert(tokens[counter++].m_Type == TokenType::KW_CONST, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::KW_CHAR, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::STAR, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::IDENTIFIER, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::EQUAL, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::STRING_LITERAL, "TestTokenScanning");
				Log::Assert(tokens[counter++].m_Type == TokenType::SEMICOLON, "TestTokenScanning");
			}
			Log::Assert(tokens[counter++].m_Type == TokenType::RIGHT_CURLY_BRACKET, "TestTokenScanning");

			// Line 183
			Log::Assert(tokens[counter++].m_Type == TokenType::MACRO_ENDIF, "TestTokenScanning");
		}
	}
}