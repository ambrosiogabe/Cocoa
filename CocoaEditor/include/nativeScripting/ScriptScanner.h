#pragma once
#include "externalLibs.h"
#include "cocoa/core/Core.h"
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

		inline Token GenerateToken(TokenType m_Type, std::string lexeme) { return Token{ m_Line, m_Column - (m_Cursor - m_Start), m_Type, lexeme }; }

		inline Token GenerateErrorToken() { return Token{ m_Line, m_Column - (m_Cursor - m_Start), TokenType::ERROR_TYPE, "" }; }

		inline Token GenerateWhitespaceToken() { return Token{ -1, -1, TokenType::WHITESPACE, "" }; }

		inline Token GenerateCommentToken() { return Token{ -1, -1, TokenType::COMMENT, "" }; }

	private:
		const std::map<std::string, TokenType> keywords = {
			// Custom Keywords for Cocoa Engine
			{ "UPROPERTY",     TokenType::UPROPERTY },
			{ "UCLASS",        TokenType::UCLASS },
			{ "USTRUCT",       TokenType::USTRUCT },
			{ "UFUNCTION",     TokenType::UFUNCTION },

			// Standard C++ Keywords (up to a subset of C++17)
			{ "alignas",       TokenType::KW_ALIGN_AS },
			{ "alignof",       TokenType::KW_ALIGN_OF },
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
			{ "private",       TokenType::KW_PRIVATE },
			{ "protected",     TokenType::KW_PROTECTED },
			{ "public",        TokenType::KW_PUBLIC },
			{ "return",        TokenType::KW_RETURN },
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

		const std::map<TokenType, std::string> tokenTypeToString = {
			// Custom Keywords for Cocoa Engine
			{ TokenType::UPROPERTY,        "UPROPERTY" },
			{ TokenType::UCLASS,           "UCLASS" },
			{ TokenType::USTRUCT,          "USTRUCT" },
			{ TokenType::UFUNCTION,        "UFUNCTION" },

			// Standard C++ Keywords
			{ TokenType::KW_ALIGN_AS,      "kw_alignas" },
			{ TokenType::KW_ALIGN_OF,      "kw_alignof" },
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
			{ TokenType::KW_PRIVATE,       "kw_private"},
			{ TokenType::KW_PROTECTED,     "kw_protected"},
			{ TokenType::KW_PUBLIC,        "kw_public"},
			{ TokenType::KW_RETURN,        "kw_return"},
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
			{ TokenType::SCOPE,            "ch_scope"},
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

		std::string m_FileContents;
		CPath m_Filepath;

		int m_Cursor = 0;
		int m_Line = 1;
		int m_Column = 0;
		int m_Start = 0;
	};
}
