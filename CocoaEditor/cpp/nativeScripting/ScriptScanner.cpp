#include "nativeScripting/ScriptScanner.h"

#include "cocoa/file/File.h"
#include "cocoa/util/Log.h"

namespace Cocoa
{
	// TODO: Change it so that we build 'myproject-userScripts.dll' that way we can cache all projects
	// TODO: in the engine exe directory. Then if we load new projects we can load the appropriate dll
	// TODO: And we can also try to recompile on engine start up, and if it fails we can use the cached dll
	ScriptScanner::ScriptScanner(const CPath& filepath)
		: m_Filepath(filepath)
	{
		FileHandle* fileHandle = File::OpenFile(filepath);
		m_FileContents = fileHandle->m_Data;
		File::CloseFile(fileHandle);
	}

	void ScriptScanner::DebugPrint(const std::vector<Token>& tokens, bool printLineAndCol, bool printWhitespace)
	{
		Log::Info("Tokens for file: '%s'", m_Filepath.Path.c_str());

		for (auto token : tokens)
		{
			if ((token.m_Type == TokenType::WHITESPACE || token.m_Type == TokenType::COMMENT))
			{
				if (!printWhitespace)
				{
					continue;
				}
				else
				{
					if (token.m_Type == TokenType::WHITESPACE)
						Log::Info("Whitespace");
					else if (token.m_Type == TokenType::COMMENT)
						Log::Info("Comment");
					continue;
				}
			}
			if (!printLineAndCol)
			{
				auto iter = tokenTypeToString.find(token.m_Type);
				Log::Assert(iter != tokenTypeToString.end(), "Invalid token while debug printing.");
				Log::Info("Token<%s>: %s", iter->second.c_str(), token.m_Lexeme);
			}
			else
			{
				auto iter = tokenTypeToString.find(token.m_Type);
				Log::Assert(iter != tokenTypeToString.end(), "Invalid token while debug printing.");
				Log::Info("Line: %d:%d Token<%s>: %s", token.m_Line, token.m_Column, iter->second.c_str(), token.m_Lexeme.c_str());
			}
		}
	}

	std::vector<Token> ScriptScanner::ScanTokens(bool includeWhitespace)
	{
		Log::Log("Scanning file '%s'", m_Filepath.Path.c_str());
		auto tokens = std::vector<Token>();

		m_Cursor = 0;
		while (!AtEnd())
		{
			m_Start = m_Cursor;
			Token token = ScanToken();
			if (!includeWhitespace)
			{
				if (token.m_Type == TokenType::WHITESPACE || token.m_Type == TokenType::COMMENT)
				{
					continue;
				}
			}
			if (token.m_Type != TokenType::ERROR_TYPE)
				tokens.push_back(token);
		}

		tokens.emplace_back(Token{ m_Line, m_Column, TokenType::END_OF_FILE, "EOF" });
		return tokens;
	}

	Token ScriptScanner::ScanToken()
	{
		char c = Advance();
		switch (c)
		{
			// Single character tokens
		case '(': return GenerateToken(TokenType::LEFT_PAREN, "(");
		case ')': return GenerateToken(TokenType::RIGHT_PAREN, ")");
		case '{': return GenerateToken(TokenType::LEFT_CURLY_BRACKET, "{");
		case '}': return GenerateToken(TokenType::RIGHT_CURLY_BRACKET, "}");
		case ';': return GenerateToken(TokenType::SEMICOLON, ";");
		case '[': return GenerateToken(TokenType::LEFT_BRACKET, "[");
		case ']': return GenerateToken(TokenType::RIGHT_BRACKET, "]");
		case '?': return GenerateToken(TokenType::QUESTION, "?");
		case '~': return GenerateToken(TokenType::TILDE, "~");
		case ',': return GenerateToken(TokenType::COMMA, ",");
		case '"': return String();
		case '\'': return Character();
		case ':':
		{
			if (Match(':'))
			{
				return GenerateToken(TokenType::SCOPE, "::");
			}
			return GenerateToken(TokenType::COLON, ":");
		}
		case '<':
		{
			if (Match('<'))
			{
				if (Match('='))
				{
					return GenerateToken(TokenType::LEFT_SHIFT_EQUAL, "<<=");
				}
				return GenerateToken(TokenType::LEFT_SHIFT, "<<");
			}
			if (Match('='))
			{
				return GenerateToken(TokenType::LESS_THAN_EQ, "<=");
			}
			return GenerateToken(TokenType::LEFT_ANGLE_BRACKET, "<");
		}
		case '>':
		{
			if (Match('>'))
			{
				if (Match('='))
				{
					return GenerateToken(TokenType::RIGHT_SHIFT_EQUAL, ">>=");
				}
				return GenerateToken(TokenType::RIGHT_SHIFT, ">>");
			}
			if (Match('='))
			{
				return GenerateToken(TokenType::GREATER_THAN_EQ, ">=");
			}
			return GenerateToken(TokenType::RIGHT_ANGLE_BRACKET, ">");
		}
		case '*':
		{
			if (Match('='))
			{
				return GenerateToken(TokenType::STAR_EQUAL, "*=");
			}
			return GenerateToken(TokenType::STAR, "*");
		}
		case '!':
		{
			if (Match('='))
			{
				return GenerateToken(TokenType::BANG_EQUAL, "!=");
			}
			return GenerateToken(TokenType::BANG, "!");
		}
		case '^':
		{
			if (Match('='))
			{
				return GenerateToken(TokenType::CARET_EQUAL, "^=");
			}
			return GenerateToken(TokenType::CARET, "^");
		}
		case '%':
		{
			if (Match('='))
			{
				return GenerateToken(TokenType::MODULO_EQUAL, "%=");
			}
			return GenerateToken(TokenType::MODULO, "%");
		}
		case '&':
		{
			if (Match('&'))
			{
				return GenerateToken(TokenType::LOGICAL_AND, "&&");
			}
			if (Match('='))
			{
				return GenerateToken(TokenType::AND_EQUAL, "&=");
			}
			return GenerateToken(TokenType::AND, "&");
		}
		case '=':
		{
			if (Match('='))
			{
				return GenerateToken(TokenType::EQUAL_EQUAL, "==");
			}
			return GenerateToken(TokenType::EQUAL, "=");
		}
		case '|':
		{
			if (Match('='))
			{
				return GenerateToken(TokenType::PIPE_EQUAL, "|=");
			}
			if (Match('|'))
			{
				return GenerateToken(TokenType::LOGICAL_OR, "||");
			}
			return GenerateToken(TokenType::PIPE, "|");
		}
		case '.':
		{
			if (IsDigit(Peek()))
			{
				return Number(c);
			}
			if (Match('*'))
			{
				return GenerateToken(TokenType::POINTER_TO_MEMBER, ".*");
			}
			return GenerateToken(TokenType::DOT, ".");
		}
		case '+':
		{
			if (Match('+'))
			{
				return GenerateToken(TokenType::PLUS_PLUS, "++");
			}
			if (Match('='))
			{
				return GenerateToken(TokenType::PLUS_EQUAL, "+=");
			}
			return GenerateToken(TokenType::PLUS, "+");
		}
		case '-':
		{
			if (Match('-'))
			{
				return GenerateToken(TokenType::MINUS_MINUS, "--");
			}
			if (Match('='))
			{
				return GenerateToken(TokenType::MINUS_EQUAL, "-=");
			}
			if (Match('>'))
			{
				if (Match('*'))
				{
					return GenerateToken(TokenType::POINTER_TO_MEMBER, "->*");
				}
				return GenerateToken(TokenType::ARROW, "->");
			}
			return GenerateToken(TokenType::MINUS, "-");
		}
		case '/':
		{
			if (Match('/'))
			{
				while (Peek() != '\n' && !AtEnd())
					Advance();
				return GenerateCommentToken();
			}
			if (Match('*'))
			{
				while (!AtEnd() && Peek() != '*' && PeekNext() != '/')
				{
					c = Advance();
					if (c == '\n')
					{
						m_Column = 0;
						m_Line++;
					}
				}

				// Consume */
				if (!AtEnd()) Match('*');
				if (!AtEnd()) Match('/');
				return GenerateCommentToken();
			}
			if (Match('='))
			{
				return GenerateToken(TokenType::DIV_EQUAL, "/=");
			}
			return GenerateToken(TokenType::DIV, "/");
		}
		case 'u':
		case 'U':
		case 'L':
		{
			// I can't Match these characters because if it's an identifier
			// that starts with u8 I don't want to accidentally consume part of
			// it before going to the identifier function
			if (c == 'u' && Peek() == '8')
			{
				if (PeekNext() == '\'')
				{
					Match('8'); Match('\'');
					return Character();
				}
				if (PeekNext() == '"')
				{
					Match('8'); Match('"');
					return String();
				}
				if (PeekNext() == 'R' && PeekNextNext() == '"')
				{
					Match('8'); Match('R'); Match('"');
					return String(true);
				}
			}
			if (c == 'u')
			{
				if (Peek() == '\'')
				{
					Match('\'');
					return Character();
				}
				if (Peek() == '"')
				{
					Match('"');
					return String();
				}
				if (Peek() == 'R' && PeekNext() == '"')
				{
					Match('R'); Match('"');
					return String(true);
				}
			}
			if (c == 'U')
			{
				if (Peek() == '\'')
				{
					Match('\'');
					return Character();
				}
				if (Peek() == '"')
				{
					Match('"');
					return String();
				}
				if (Peek() == 'R' && PeekNext() == '"')
				{
					Match('R'); Match('"');
					return String(true);
				}
			}
			if (c == 'L')
			{
				if (Peek() == '\'')
				{
					Match('\'');
					return Character();
				}
				if (Peek() == '"')
				{
					Match('"');
					return String();
				}
				if (Peek() == 'R' && PeekNext() == '"')
				{
					Match('R'); Match('"');
					return String(true);
				}
			}
			return PropertyIdentifier();
		}
		case 'R':
		{
			if (Peek() == '"')
			{
				Match('R'); Match('"');
				return String(true);
			}
			return PropertyIdentifier();
		}
		case ' ':
		case '\r':
		case '\t':
			// Ignore whitespace
			return GenerateWhitespaceToken();
		case '\n':
			// Record the new line, then continue
			m_Column = 0;
			m_Line++;
			return GenerateWhitespaceToken();
		default:
			if (IsDigit(c))
			{
				return Number(c);
			}
			if (IsAlpha(c) || c == '_')
			{
				return PropertyIdentifier();
			}
			if (c == '#')
			{
				return Macro();
			}
			break;
		}

		return GenerateErrorToken();
	}

	Token ScriptScanner::Macro()
	{
		while (IsAlphaNumeric(Peek())) Advance();

		std::string text = std::string(m_FileContents.substr(m_Start, m_Cursor - m_Start));
		auto iter = keywords.find(text);
		if (iter != keywords.end())
		{
			return Token{ m_Line, m_Column - (m_Cursor - m_Start), iter->second, text };
		}

		return GenerateErrorToken();
	}

	Token ScriptScanner::PropertyIdentifier()
	{
		while (IsAlphaNumeric(Peek()) || Peek() == '_') Advance();

		std::string text = std::string(m_FileContents.substr(m_Start, m_Cursor - m_Start));
		TokenType type = TokenType::IDENTIFIER;
		auto iter = keywords.find(text);
		if (iter != keywords.end())
		{
			type = iter->second;
		}

		return Token{ m_Line, m_Column - (m_Cursor - m_Start), type, text };
	}

	Token ScriptScanner::Number(char firstDigit)
	{
		bool isHexadecimal = false;
		bool isOctal = false;
		bool isBinary = false;
		if (firstDigit == '0')
		{
			// If the number starts with a 0x, it's hex, otherwise
			// if it starts with a 0 and it's not followed by a '.' it's octal
			if (Match('x') || Match('X'))
			{
				isHexadecimal = true;
			}
			else if (Match('b') || Match('B'))
			{
				isBinary = true;
			}
			else if (Peek() != '.')
			{
				isOctal = true;
			}
		}

		if (isHexadecimal)
		{
			return NumberHexadecimal();
		}
		else if (isOctal)
		{
			return NumberOctal();
		}
		else if (isBinary)
		{
			return NumberBinary();
		}
		return NumberDecimal(firstDigit);
	}

	Token ScriptScanner::NumberDecimal(char firstDigit)
	{
		while (firstDigit != '.' && IsDigit(Peek(), true))
		{
			Advance();
		}

		bool isFloatingPoint = false;
		if (Match('.') || firstDigit == '.')
		{
			isFloatingPoint = true;

			while (IsDigit(Peek(), true))
			{
				Advance();
			}
		}

		if (IsSameChar(Peek(), 'e', 'E') &&
			(IsDigit(PeekNext()) ||
				(
					(PeekNext() == '-' && IsDigit(PeekNextNext())) ||
					(PeekNext() == '+' && IsDigit(PeekNextNext()))
					)
				)
			)
		{
			isFloatingPoint = true;
			Advance();
			Advance();
			while (IsDigit(Peek(), true))
			{
				Advance();
			}

			if ((Peek() == '-' || Peek() == '+') && IsDigit(PeekNext()))
			{
				Advance();
				while (IsDigit(Peek())) Advance();
			}

			if (Peek() == '.')
			{
				Log::Error("Unexpected number literal at %d col:%d", m_Line, m_Column);
				return GenerateErrorToken();
			}
		}

		// This bit is just to consume the trailing 'f' or 'l'
		if (isFloatingPoint)
		{
			if (IsSameChar(Peek(), 'f', 'F'))
			{
				Match(Peek());
			}
			else if (IsSameChar(Peek(), 'l', 'L'))
			{
				Match(Peek());
			}
		}

		if (!isFloatingPoint)
		{
			ConsumeTrailingUnsignedLong();

			return Token{ m_Line, m_Column - (m_Cursor - m_Start), TokenType::INTEGER_LITERAL, m_FileContents.substr(m_Start, m_Cursor - m_Start) };
		}

		return Token{ m_Line, m_Column - (m_Cursor - m_Start), TokenType::FLOATING_POINT_LITERAL, m_FileContents.substr(m_Start, m_Cursor - m_Start) };
	}

	Token ScriptScanner::NumberHexadecimal()
	{
		while (IsHexDigit(Peek(), true)) Advance();
		ConsumeTrailingUnsignedLong();

		return Token{ m_Line, m_Column - (m_Cursor - m_Start), TokenType::INTEGER_LITERAL, m_FileContents.substr(m_Start, m_Cursor - m_Start) };
	}

	Token ScriptScanner::NumberBinary()
	{
		while (Peek() == '0' || Peek() == '1' || Peek() == '\'') Advance();
		ConsumeTrailingUnsignedLong();

		return Token{ m_Line, m_Column - (m_Cursor - m_Start), TokenType::INTEGER_LITERAL, m_FileContents.substr(m_Start, m_Cursor - m_Start) };
	}

	Token ScriptScanner::NumberOctal()
	{
		while (Peek() >= '0' && Peek() <= '7' || Peek() == '\'') Advance();
		ConsumeTrailingUnsignedLong();

		return Token{ m_Line, m_Column - (m_Cursor - m_Start), TokenType::INTEGER_LITERAL, m_FileContents.substr(m_Start, m_Cursor - m_Start) };
	}

	void ScriptScanner::ConsumeTrailingUnsignedLong()
	{
		// consume up to 3 'u' or 'l' characters
		if (IsSameChar(Peek(), 'u', 'U'))
		{
			Match(Peek());
		}

		if (IsSameChar(Peek(), 'l', 'L'))
		{
			Match(Peek());
		}

		if (IsSameChar(Peek(), 'l', 'L'))
		{
			Match(Peek());
		}
	}


	Token ScriptScanner::Character()
	{
		// The first apostrophe ' has already been consume at this point
		while (Peek() != '\'' && !AtEnd())
		{
			if (Peek() == '\n')
			{
				Log::Warning("Invalid character literal encountered while parsing at line: %d:%d", m_Line, m_Column);
				m_Line++;
				m_Column = -1;
				break;
			}
			// Skip over any escaped quotes
			if (Peek() == '\\' && PeekNext() == '\'')
			{
				Advance();
			}
			// Skip over escaped back slashes so that it doesn't accidentally skip an end quote
			if (Peek() == '\\' && PeekNext() == '\\')
			{
				Advance();
			}
			Advance();
		}

		if (AtEnd())
		{
			// TODO: This might not need to be here
			Log::Warning("Unexpected character literal at line %d:%d", m_Line, m_Column);
			return GenerateErrorToken();
		}

		Advance();

		std::string value = m_FileContents.substr(m_Start, m_Cursor - m_Start);
		return Token{ m_Line, m_Column - (m_Cursor - m_Start), TokenType::CHARACTER_LITERAL, value };
	}

	Token ScriptScanner::String(bool isRawStringLiteral)
	{
		if (isRawStringLiteral)
		{
			int matchStart = m_Cursor;
			while (Peek() != '(')
			{
				Advance();
			}
			std::string strToMatch = ")";
			if (m_Cursor > matchStart)
			{
				strToMatch += m_FileContents.substr(matchStart, m_Cursor - matchStart);
			}

			while (!AtEnd())
			{
				if (Peek() == ')')
				{
					std::string fileSubstr = m_FileContents.substr(m_Cursor, strToMatch.size());
					if (fileSubstr == strToMatch)
					{
						for (int i = 0; i < strToMatch.size(); i++)
						{
							Advance();
						}
						break;
					}
				}
				Advance();
			}
		}
		else
		{

			while (Peek() != '"' && !AtEnd())
			{
				if (Peek() == '\n')
				{
					m_Line++;
					m_Column = -1;
				}
				Advance();
			}
		}

		if (AtEnd())
		{
			Log::Error("Unexpected string literal at %d col:%d", m_Line, m_Column);
			return GenerateErrorToken();
		}

		Advance();

		std::string value = m_FileContents.substr(m_Start, m_Cursor - m_Start);
		return Token{ m_Line, m_Column - (m_Cursor - m_Start), TokenType::STRING_LITERAL, value };
	}

	char ScriptScanner::Advance()
	{
		char c = m_FileContents[m_Cursor];
		m_Cursor++;
		m_Column++;
		return c;
	}

	char ScriptScanner::Peek()
	{
		if (AtEnd()) return '\0';
		return m_FileContents[m_Cursor];
	}

	char ScriptScanner::PeekNext()
	{
		if (AtEnd() || m_Cursor == m_FileContents.size() - 1) return '\0';
		return m_FileContents[m_Cursor + 1];
	}

	char ScriptScanner::PeekNextNext()
	{
		if (AtEnd() || m_Cursor == m_FileContents.size() - 1 || m_Cursor == m_FileContents.size() - 2) return '\0';
		return m_FileContents[m_Cursor + 2];
	}

	bool ScriptScanner::Match(char expected)
	{
		if (AtEnd()) return false;
		if (m_FileContents[m_Cursor] != expected) return false;

		m_Cursor++;
		m_Column++;
		return true;
	}
}