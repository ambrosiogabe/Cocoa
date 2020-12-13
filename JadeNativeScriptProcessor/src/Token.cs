using System;

namespace Reflection
{
	public enum TokenType
	{
		PROPERTY, FUNCTION_PROP, CLASS_PROP, STRUCT_PROP,
		STRUCT_KW, CLASS_KW, 

		LEFT_PAREN, RIGHT_PAREN, HASHTAG, LEFT_BRACKET, RIGHT_BRACKET,
		SEMICOLON, EQUAL, PLUS_EQUAL, MINUS_EQUAL, TIMES_EQUAL, DIV_EQUAL,
		MODULO_EQUAL, CARET_EQUAL, AMPERSAND_EQUAL, BAR_EQUAL, RIGHT_SHIFT_EQUAL,
		LEFT_SHIFT_EQUAL, EQUAL_EQUAL, BANG_EQUAL, LESS_THAN_EQUAL, GREATER_THAN_EQUAL,
		
		CONST, IDENTIFIER, AUTO,

		STAR, REF, LEFT_ANGLE_BRACKET, RIGHT_ANGLE_BRACKET, 

		STRING_LITERAL, NUMBER, TRUE, FALSE,

		EOF
	}

	public class Token
	{
		public TokenType Type { get; set; }

		public string Lexeme { get; set; }

		public Object Literal { get; set; }

		public int Line { get; set; }

		public int Column { get; set; }
	}
}
