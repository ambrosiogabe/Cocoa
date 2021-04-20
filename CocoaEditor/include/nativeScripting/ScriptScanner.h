#pragma once
#include "externalLibs.h"
#include "cocoa/core/Core.h"
#include "cocoa/core/CString.h"
#include "cocoa/file/CPath.h"

#include "nativeScripting/CppTokens.h"

namespace Cocoa
{
	class ScriptScanner
	{
	public:
		ScriptScanner(const CPath& filepath);

		std::vector<Token> ScanTokens(bool includeWhitespace=false);
		void DebugPrint(const std::vector<Token>& tokens, bool printLineAndCol=true, bool printWhitespace=false);

		static std::string TokenName(TokenType type);

	private:
		Token ScanToken();
		Token Macro();
		Token PropertyIdentifier();
		Token Number(char firstDigit);
		Token NumberDecimal(char firstDigit);
		Token NumberBinary();
		Token NumberHexadecimal();
		Token NumberOctal();

		Token Character();
		Token String(bool isRawStringLiteral=false);

		char Advance();
		char Peek();
		char PeekNext();
		char PeekNextNext();
		bool Match(char expected);
		void ConsumeTrailingUnsignedLong();

	private:
		inline bool IsDigit(char c, bool acceptApostrophe=false) const { return c >= '0' && c <= '9' || (acceptApostrophe && c == '\''); }

		inline bool IsAlphaNumeric(char c) const { return IsAlpha(c) || IsDigit(c); }

		inline bool IsHexDigit(char c, bool acceptApostrophe) const { return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || (acceptApostrophe && c == '\''); }

		inline bool IsAlpha(char c) const { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }

		inline bool IsSameChar(char c, char lowerCase, char upperCase) { return c == lowerCase || c == upperCase; }

		inline bool AtEnd() const { return m_Cursor == m_FileContents.size(); }

		inline Token GenerateToken(TokenType m_Type, std::string lexeme) { 
			return Token{ 
				m_Line, 
				m_Column - (m_Cursor - m_Start), 
				m_Type, 
				NCString::Create(lexeme)
			}; 
		}

		inline Token GenerateErrorToken() { 
			return Token{ 
				m_Line, 
				m_Column - (m_Cursor - m_Start), 
				TokenType::ERROR_TYPE, 
				NCString::Create("")
			}; 
		}

		inline Token GenerateWhitespaceToken() { 
			return Token{ 
				-1, 
				-1, 
				TokenType::WHITESPACE, 
				NCString::Create("")
			}; 
		}

		inline Token GenerateCommentToken() { 
			return Token{ 
				-1, 
				-1, 
				TokenType::COMMENT, 
				NCString::Create("")
			}; 
		}

	private:
		std::string m_FileContents;
		CPath m_Filepath;

		int m_Cursor = 0;
		int m_Line = 1;
		int m_Column = 0;
		int m_Start = 0;
	};
}
