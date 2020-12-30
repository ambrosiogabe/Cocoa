#include "nativeScripting/ScriptScanner.h"

#include "jade/file/IFile.h"
#include "jade/util/Log.h"

namespace Jade
{
	ScriptScanner::ScriptScanner(const JPath& filepath)
		: m_Filepath(filepath)
	{
		File* fileHandle = IFile::OpenFile(filepath);
		m_FileContents = fileHandle->m_Data;
		IFile::CloseFile(fileHandle);
	}

	std::vector<Token> ScriptScanner::ScanTokens()
	{
		Log::Log("Scanning file '%s'", m_Filepath.Filepath());
		auto tokens = std::vector<Token>();

		m_Cursor = 0;
		while (!AtEnd())
		{
			m_Start = m_Cursor;
			Token token = ScanToken();
			if (token.m_Type != TokenType::ERROR_TYPE)
				tokens.push_back(token);
		}

		tokens.emplace_back(Token { m_Line, m_Column, TokenType::END_OF_FILE, "EOF" });
		return tokens;
	}

	Token ScriptScanner::ScanToken()
	{
		char c = Advance();
		switch (c)
		{
			// Single character tokens
		case '<': return GenerateToken(TokenType::LEFT_ANGLE_BRACKET, "<");
		case '>': return GenerateToken(TokenType::RIGHT_ANGLE_BRACKET, "<");
		case '*': return GenerateToken(TokenType::STAR, "*");
		case '&': return GenerateToken(TokenType::REF, "&");
		case '(': return GenerateToken(TokenType::LEFT_PAREN, "(");
		case ')': return GenerateToken(TokenType::RIGHT_PAREN, ")");
		case '#': return GenerateToken(TokenType::HASHTAG, "#");
		case '{': return GenerateToken(TokenType::LEFT_BRACKET, "{");
		case '}': return GenerateToken(TokenType::RIGHT_BRACKET, "}");
		case ';': return GenerateToken(TokenType::SEMICOLON, ";");
		case '=': return GenerateToken(TokenType::EQUAL, "=");
		case ':': return GenerateToken(TokenType::COLON, ":");
		case '"': return String();
			// Whitespace
		case '/':
		{
			if (Match('/'))
			{
				while (Peek() != '\n' && !AtEnd())
					Advance();
				return GenerateErrorToken();
			}
			else if (Match('*'))
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
				return GenerateErrorToken();
			}
			break;
		}
		case ' ':
		case '\r':
		case '\t':
			// Ignore whitespace
			return GenerateErrorToken();
		case '\n':
			m_Column = 0;
			m_Line++;
			return GenerateErrorToken();
		default:
			if (IsAlpha(c))
			{
				return PropertyIdentifier();
			}
			break;
		}

		return GenerateErrorToken();
	}

	Token ScriptScanner::PropertyIdentifier()
	{
		while (IsAlphaNumeric(Peek())) Advance();

		std::string text = m_FileContents.substr(m_Start, m_Cursor - m_Start);
		TokenType type = TokenType::IDENTIFIER;
		auto iter = keywords.find(text);
		if (iter != keywords.end())
		{
			type = iter->second;
		}

		return Token{ m_Line, m_Column, type, text };
	}

	Token ScriptScanner::Number()
	{
		while (IsDigit(Peek())) Advance();

		bool hasE = false;
		if (Peek() == '.' && (IsDigit(PeekNext()) || (PeekNext() == 'e' && IsDigit(PeekNextNext()))
			|| (PeekNext() == 'E' && IsDigit(PeekNextNext()))))
		{
			Advance();

			while (IsDigit(Peek()))
			{
				Advance();
			}

			if ((Peek() == 'e' || Peek() == 'E') && (IsDigit(PeekNext()) ||
				((PeekNext() == '-' && IsDigit(PeekNextNext())) || (PeekNext() == '+' && IsDigit(PeekNextNext())))))
			{
				Advance();
				while (IsDigit(Peek())) Advance();

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
		}

		if ((Peek() == 'e' || Peek() == 'E') && (IsDigit(PeekNext()) ||
			((PeekNext() == '-' && IsDigit(PeekNextNext())) || (PeekNext() == '+' && IsDigit(PeekNextNext())))))
		{
			Advance();
			while (IsDigit(Peek())) Advance();

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

		return Token{ m_Line, m_Column, TokenType::NUMBER, m_FileContents.substr(m_Start, m_Cursor - m_Start) };
	}

	Token ScriptScanner::String()
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

		if (AtEnd())
		{
			Log::Error("Unexpected string literal at %d col:%d", m_Line, m_Column);
			return GenerateErrorToken();
		}

		Advance();

		std::string value = m_FileContents.substr(m_Start, m_Cursor - m_Start);
		return Token{ m_Column, m_Line, TokenType::STRING_LITERAL, value };
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
		return m_FileContents[m_Cursor + 1];
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