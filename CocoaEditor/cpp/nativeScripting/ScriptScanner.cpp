#include "nativeScripting/ScriptScanner.h"

#include "cocoa/file/File.h"
#include "cocoa/util/Log.h"

namespace Cocoa
{
	// TODO: Change it so that we build 'myproject-userScripts.dll' that way we can cache all projects
	// TODO: in the engine exe directory. Then if we load new projects we can load the appropriate dll
	// TODO: And we can also try to recompile on engine start up, and if it fails we can use the cached dll
	static const std::map<std::string, TokenType> keywords = {
		// Custom Keywords for Cocoa Engine
		{ "UPROPERTY",     TokenType::UPROPERTY },
		{ "UCLASS",        TokenType::UCLASS },
		{ "USTRUCT",       TokenType::USTRUCT },
		{ "UFUNCTION",     TokenType::UFUNCTION },

		// Standard C++ Keywords (up to a subset of C++17)
		{ "alignas",       TokenType::KW_ALIGN_AS },
		{ "alignof",       TokenType::KW_ALIGN_OF },
		{ "asm",           TokenType::KW_ASM },
		{ "auto",          TokenType::KW_AUTO },
		{ "bool",          TokenType::KW_BOOL },
		{ "break",         TokenType::KW_BREAK },
		{ "case",          TokenType::KW_CASE },
		{ "catch",         TokenType::KW_CATCH },
		{ "char",          TokenType::KW_CHAR },
		{ "char8_t",       TokenType::KW_CHAR8_T },
		{ "char16_t",      TokenType::KW_CHAR16_T },
		{ "char32_t",      TokenType::KW_CHAR32_T },
		{ "class",         TokenType::KW_CLASS },
		{ "const",         TokenType::KW_CONST },
		{ "const_cast",    TokenType::KW_CONST_CAST },
		{ "constexpr",     TokenType::KW_CONST_EXPR },
		{ "continue",      TokenType::KW_CONTINUE },
		{ "decltype",      TokenType::KW_DECLTYPE },
		{ "default",       TokenType::KW_DEFAULT },
		{ "delete",        TokenType::KW_DELETE },
		{ "do",            TokenType::KW_DO },
		{ "double",        TokenType::KW_DOUBLE },
		{ "dynamic_cast",  TokenType::KW_DYNAMIC_CAST },
		{ "else",          TokenType::KW_ELSE },
		{ "enum",          TokenType::KW_ENUM },
		{ "explicit",      TokenType::KW_EXPLICIT },
		{ "extern",        TokenType::KW_EXTERN },
		{ "false",         TokenType::KW_FALSE },
		{ "final",         TokenType::KW_FINAL },
		{ "float",         TokenType::KW_FLOAT },
		{ "for",           TokenType::KW_FOR },
		{ "friend",        TokenType::KW_FRIEND },
		{ "goto",          TokenType::KW_GOTO },
		{ "if",            TokenType::KW_IF },
		{ "inline",        TokenType::KW_INLINE },
		{ "int",           TokenType::KW_INT },
		{ "long",          TokenType::KW_LONG },
		{ "mutable",       TokenType::KW_MUTABLE },
		{ "namespace",     TokenType::KW_NAMESPACE },
		{ "new",           TokenType::KW_NEW },
		{ "noexcept",      TokenType::KW_NOEXCEPT },
		{ "nullptr",       TokenType::KW_NULLPTR },
		{ "operator",      TokenType::KW_OPERATOR },
		{ "override",      TokenType::KW_OVERRIDE },
		{ "private",       TokenType::KW_PRIVATE },
		{ "protected",     TokenType::KW_PROTECTED },
		{ "public",        TokenType::KW_PUBLIC },
		{ "reinterpret_cast", TokenType::KW_REINTERPRET_CAST },
		{ "return",        TokenType::KW_RETURN },
		{ "register",      TokenType::KW_REGISTER },
		{ "short",         TokenType::KW_SHORT },
		{ "signed",        TokenType::KW_SIGNED },
		{ "sizeof",        TokenType::KW_SIZEOF },
		{ "static",        TokenType::KW_STATIC },
		{ "static_assert", TokenType::KW_STATIC_ASSERT },
		{ "static_cast",   TokenType::KW_STATIC_CAST },
		{ "struct",        TokenType::KW_STRUCT },
		{ "switch",        TokenType::KW_SWITCH },
		{ "template",      TokenType::KW_TEMPLATE },
		{ "this",          TokenType::KW_THIS },
		{ "thread_local",  TokenType::KW_THREAD_LOCAL },
		{ "throw",         TokenType::KW_THROW },
		{ "true",          TokenType::KW_TRUE },
		{ "try",           TokenType::KW_TRY },
		{ "typedef",       TokenType::KW_TYPEDEF },
		{ "typeid",        TokenType::KW_TYPEID },
		{ "typename",      TokenType::KW_TYPENAME },
		{ "union",         TokenType::KW_UNION },
		{ "unsigned",      TokenType::KW_UNSIGNED },
		{ "using",         TokenType::KW_USING },
		{ "virtual",       TokenType::KW_VIRTUAL },
		{ "void",          TokenType::KW_VOID },
		{ "volatile",      TokenType::KW_VOLATILE },
		{ "wchar_t",       TokenType::KW_WCHAR_T },
		{ "while",         TokenType::KW_WHILE },

		// Macros
		{ "#include",    TokenType::MACRO_INCLUDE },
		{ "#ifdef",      TokenType::MACRO_IFDEF },
		{ "#ifndef",     TokenType::MACRO_IFNDEF },
		{ "#define",     TokenType::MACRO_DEFINE },
		{ "#undefine",   TokenType::MACRO_UNDEF },
		{ "#if",         TokenType::MACRO_IF },
		{ "#elif",       TokenType::MACRO_ELIF },
		{ "#endif",      TokenType::MACRO_ENDIF },
		{ "#else",       TokenType::MACRO_ELSE },
		{ "#error",      TokenType::MACRO_ERROR },
		{ "#line",       TokenType::MACRO_LINE },
		{ "#pragma",     TokenType::MACRO_PRAGMA },
		{ "#region",     TokenType::MACRO_REGION },
		{ "#using",      TokenType::MACRO_USING }
	};

	static const std::map<TokenType, std::string> tokenTypeToString = {
		// Custom Keywords for Cocoa Engine
		{ TokenType::UPROPERTY,        "UPROPERTY" },
		{ TokenType::UCLASS,           "UCLASS" },
		{ TokenType::USTRUCT,          "USTRUCT" },
		{ TokenType::UFUNCTION,        "UFUNCTION" },

		// Standard C++ Keywords
		{ TokenType::KW_ALIGN_AS,      "kw_alignas" },
		{ TokenType::KW_ALIGN_OF,      "kw_alignof" },
		{ TokenType::KW_ASM,           "kw_asm" },
		{ TokenType::KW_AUTO,          "kw_auto" },
		{ TokenType::KW_BOOL,          "kw_bool" },
		{ TokenType::KW_BREAK,         "kw_break" },
		{ TokenType::KW_CASE,          "kw_case" },
		{ TokenType::KW_CATCH,         "kw_catch" },
		{ TokenType::KW_CHAR,          "kw_char" },
		{ TokenType::KW_CHAR8_T,       "kw_char8_t" },
		{ TokenType::KW_CHAR16_T,      "kw_char16_t" },
		{ TokenType::KW_CHAR32_T,      "kw_char32_t" },
		{ TokenType::KW_CLASS,         "kw_class" },
		{ TokenType::KW_CONST,         "kw_const" },
		{ TokenType::KW_CONST_CAST,    "kw_const_cast" },
		{ TokenType::KW_CONST_EXPR,    "kw_constexpr" },
		{ TokenType::KW_CONTINUE,      "kw_continue" },
		{ TokenType::KW_DECLTYPE,      "kw_decltype"},
		{ TokenType::KW_DEFAULT,       "kw_default"},
		{ TokenType::KW_DELETE,        "kw_delete"},
		{ TokenType::KW_DO,            "kw_do"},
		{ TokenType::KW_DOUBLE,        "kw_double"},
		{ TokenType::KW_DYNAMIC_CAST,  "kw_dynamic_cast"},
		{ TokenType::KW_ELSE,          "kw_else"},
		{ TokenType::KW_ENUM,          "kw_enum"},
		{ TokenType::KW_EXPLICIT,      "kw_explicit"},
		{ TokenType::KW_EXTERN,        "kw_extern"},
		{ TokenType::KW_FALSE,         "kw_false"},
		{ TokenType::KW_FINAL,         "kw_final" },
		{ TokenType::KW_FLOAT,         "kw_float"},
		{ TokenType::KW_FOR,           "kw_for"},
		{ TokenType::KW_FRIEND,        "kw_friend"},
		{ TokenType::KW_GOTO,          "kw_goto"},
		{ TokenType::KW_IF,            "kw_if"},
		{ TokenType::KW_INLINE,        "kw_inline"},
		{ TokenType::KW_INT,           "kw_int"},
		{ TokenType::KW_LONG,          "kw_long"},
		{ TokenType::KW_MUTABLE,       "kw_mutable"},
		{ TokenType::KW_NAMESPACE,     "kw_namespace"},
		{ TokenType::KW_NEW,           "kw_new"},
		{ TokenType::KW_NOEXCEPT,      "kw_noexcept"},
		{ TokenType::KW_NULLPTR,       "kw_nullptr"},
		{ TokenType::KW_OPERATOR,      "kw_operator"},
		{ TokenType::KW_OVERRIDE,      "kw_override" },
		{ TokenType::KW_PRIVATE,       "kw_private"},
		{ TokenType::KW_PROTECTED,     "kw_protected"},
		{ TokenType::KW_PUBLIC,        "kw_public"},
		{ TokenType::KW_REINTERPRET_CAST, "kw_reinterpret_cast" },
		{ TokenType::KW_RETURN,        "kw_return"},
		{ TokenType::KW_REGISTER,      "kw_register" },
		{ TokenType::KW_SHORT,         "kw_short"},
		{ TokenType::KW_SIGNED,        "kw_signed"},
		{ TokenType::KW_SIZEOF,        "kw_sizeof"},
		{ TokenType::KW_STATIC,        "kw_static"},
		{ TokenType::KW_STATIC_ASSERT, "kw_static_assert"},
		{ TokenType::KW_STATIC_CAST,   "kw_static_cast"},
		{ TokenType::KW_STRUCT,        "kw_struct"},
		{ TokenType::KW_SWITCH,        "kw_switch"},
		{ TokenType::KW_TEMPLATE,      "kw_template"},
		{ TokenType::KW_THIS,          "kw_this"},
		{ TokenType::KW_THREAD_LOCAL,  "kw_thread_local"},
		{ TokenType::KW_THROW,         "kw_throw"},
		{ TokenType::KW_TRUE,          "kw_true"},
		{ TokenType::KW_TRY,           "kw_try"},
		{ TokenType::KW_TYPEDEF,       "kw_typedef"},
		{ TokenType::KW_TYPEID,        "kw_typeid"},
		{ TokenType::KW_TYPENAME,      "kw_typename"},
		{ TokenType::KW_UNION,         "kw_union"},
		{ TokenType::KW_UNSIGNED,      "kw_unsigned"},
		{ TokenType::KW_USING,         "kw_using"},
		{ TokenType::KW_VIRTUAL,       "kw_virtual"},
		{ TokenType::KW_VOID,          "kw_void",},
		{ TokenType::KW_VOLATILE,      "kw_volatile"},
		{ TokenType::KW_WCHAR_T,       "kw_wchar_t"},
		{ TokenType::KW_WHILE,         "kw_while"},
		{ TokenType::IDENTIFIER,       "identifier"},
		{ TokenType::DOT,              "ch_dot"},
		{ TokenType::ARROW,            "ch_arrow"},
		{ TokenType::LEFT_BRACKET,     "ch_left_bracket"},
		{ TokenType::RIGHT_BRACKET,    "ch_right_bracket"},
		{ TokenType::LEFT_PAREN,       "ch_left_paren"},
		{ TokenType::RIGHT_PAREN,      "ch_right_paren"},
		{ TokenType::PLUS,             "ch_plus"},
		{ TokenType::PLUS_PLUS,        "ch_plus_plus"},
		{ TokenType::MINUS,            "ch_minus"},
		{ TokenType::MINUS_MINUS,      "ch_minus_minus"},
		{ TokenType::TILDE,            "ch_tilde"},
		{ TokenType::BANG,             "ch_bang"},
		{ TokenType::AND,              "ch_and"},
		{ TokenType::STAR,             "ch_star"},
		{ TokenType::DIV,              "ch_div"},
		{ TokenType::MODULO,           "ch_modulo"},
		{ TokenType::LEFT_SHIFT,       "ch_left_shift"},
		{ TokenType::RIGHT_SHIFT,      "ch_right_shift"},
		{ TokenType::LEFT_ANGLE_BRACKET,  "ch_left_angle_bracket"},
		{ TokenType::RIGHT_ANGLE_BRACKET, "ch_right_angle_bracket"},
		{ TokenType::LESS_THAN_EQ,     "ch_less_than_eq"},
		{ TokenType::GREATER_THAN_EQ,  "ch_greater_than_eq"},
		{ TokenType::EQUAL_EQUAL,      "ch_equal_equal"},
		{ TokenType::BANG_EQUAL,       "ch_bang_equal"},
		{ TokenType::EQUAL,            "ch_equal"},
		{ TokenType::CARET,            "ch_caret"},
		{ TokenType::PIPE,             "ch_pipe"},
		{ TokenType::QUESTION,         "ch_question"},
		{ TokenType::COLON,            "ch_colon"},
		{ TokenType::LOGICAL_AND,      "ch_logical_and"},
		{ TokenType::LOGICAL_OR,       "ch_logical_or"},
		{ TokenType::STAR_EQUAL,       "ch_star_equal"},
		{ TokenType::DIV_EQUAL,        "ch_div_equal"},
		{ TokenType::MODULO_EQUAL,     "ch_modulo_equal"},
		{ TokenType::PLUS_EQUAL,       "ch_plus_equal"},
		{ TokenType::MINUS_EQUAL,      "ch_minus_equal"},
		{ TokenType::LEFT_SHIFT_EQUAL, "ch_left_shift_equal"},
		{ TokenType::RIGHT_SHIFT_EQUAL, "ch_right_shift_equal"},
		{ TokenType::AND_EQUAL,        "ch_and_equal"},
		{ TokenType::PIPE_EQUAL,       "ch_pipe_equal"},
		{ TokenType::CARET_EQUAL,      "ch_caret_equal"},
		{ TokenType::COMMA,            "ch_comma"},
		{ TokenType::LEFT_CURLY_BRACKET,  "ch_left_curly_bracket"},
		{ TokenType::RIGHT_CURLY_BRACKET, "ch_right_curly_bracket"},
		{ TokenType::SEMICOLON,        "ch_semicolon"},
		{ TokenType::POINTER_TO_MEMBER, "ch_pointer_to_member"},
		{ TokenType::STRING_LITERAL,   "string_literal"},
		{ TokenType::INTEGER_LITERAL,  "integer_literal"},
		{ TokenType::FLOATING_POINT_LITERAL, "floating_point_literal"},
		{ TokenType::CHARACTER_LITERAL,"character_literal"},
		{ TokenType::COMMENT,          "comment"},
		{ TokenType::WHITESPACE,       "whitespace"},
		{ TokenType::END_OF_FILE,      "EOF"},
		{ TokenType::ERROR_TYPE,       "ERROR_TYPE"},

		// Macros
		{ TokenType::MACRO_INCLUDE, "#include" },
		{ TokenType::MACRO_IFDEF,   "#ifdef" },
		{ TokenType::MACRO_IFNDEF,  "#ifndef" },
		{ TokenType::MACRO_DEFINE,  "#define" },
		{ TokenType::MACRO_UNDEF,   "#undefine" },
		{ TokenType::MACRO_IF,      "#if" },
		{ TokenType::MACRO_ELIF,    "#elif" },
		{ TokenType::MACRO_ENDIF,   "#endif" },
		{ TokenType::MACRO_ELSE,    "#else" },
		{ TokenType::MACRO_ERROR,   "#error" },
		{ TokenType::MACRO_LINE,    "#line" },
		{ TokenType::MACRO_PRAGMA,  "#pragma" },
		{ TokenType::MACRO_REGION,  "#region" },
		{ TokenType::MACRO_USING,   "#using" }
	};

	std::string ScriptScanner::TokenName(TokenType type)
	{
		auto iter = tokenTypeToString.find(type);
		if (iter == tokenTypeToString.end())
		{
			return "ERROR";
		}

		return iter->second;
	}

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
				Log::Info("Line: %d:%d Token<%s>: %s", token.m_Line, token.m_Column, iter->second.c_str(), token.m_Lexeme.String);
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
			return Token{ m_Line, m_Column - (m_Cursor - m_Start), iter->second, NCString::Create(text) };
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

		return Token{ m_Line, m_Column - (m_Cursor - m_Start), type, NCString::Create(text) };
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

			return Token{ m_Line, m_Column - (m_Cursor - m_Start), TokenType::INTEGER_LITERAL, NCString::Create(m_FileContents.substr(m_Start, m_Cursor - m_Start)) };
		}

		return Token{ m_Line, m_Column - (m_Cursor - m_Start), TokenType::FLOATING_POINT_LITERAL, NCString::Create(m_FileContents.substr(m_Start, m_Cursor - m_Start)) };
	}

	Token ScriptScanner::NumberHexadecimal()
	{
		while (IsHexDigit(Peek(), true)) Advance();
		ConsumeTrailingUnsignedLong();

		return Token{ m_Line, m_Column - (m_Cursor - m_Start), TokenType::INTEGER_LITERAL, NCString::Create(m_FileContents.substr(m_Start, m_Cursor - m_Start)) };
	}

	Token ScriptScanner::NumberBinary()
	{
		while (Peek() == '0' || Peek() == '1' || Peek() == '\'') Advance();
		ConsumeTrailingUnsignedLong();

		return Token{ m_Line, m_Column - (m_Cursor - m_Start), TokenType::INTEGER_LITERAL, NCString::Create(m_FileContents.substr(m_Start, m_Cursor - m_Start)) };
	}

	Token ScriptScanner::NumberOctal()
	{
		while (Peek() >= '0' && Peek() <= '7' || Peek() == '\'') Advance();
		ConsumeTrailingUnsignedLong();

		return Token{ m_Line, m_Column - (m_Cursor - m_Start), TokenType::INTEGER_LITERAL, NCString::Create(m_FileContents.substr(m_Start, m_Cursor - m_Start)) };
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
		return Token{ m_Line, m_Column - (m_Cursor - m_Start), TokenType::CHARACTER_LITERAL, NCString::Create(value) };
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
		return Token{ m_Line, m_Column - (m_Cursor - m_Start), TokenType::STRING_LITERAL, NCString::Create(value) };
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