using System;
using System.Collections.Generic;
using System.IO;

namespace Reflection
{
	class Scanner
	{
		private readonly Dictionary<string, TokenType> keywords = new Dictionary<string, TokenType>
		{
			{ "UPROPERTY",   TokenType.PROPERTY },
			{ "UCLASS",      TokenType.CLASS_PROP },
			{ "USTRUCT",     TokenType.STRUCT_PROP },
			{ "UFUNCTION",   TokenType.FUNCTION_PROP },
			{ "auto",        TokenType.AUTO },
			{ "const",       TokenType.CONST },
			{ "class",       TokenType.CLASS_KW },
			{ "struct",      TokenType.STRUCT_KW }
		};

		private readonly string _fileContents;
		private readonly string _file;

		private int _cursor = 0;
		private int _line = 1;
		private int _column = 0;
		private int _start = 0;

		public Scanner(string file)
		{
			_file = file;
			_fileContents = File.ReadAllText(file);
		}

		public List<Token> ScanTokens()
		{
			Console.WriteLine($"Scanning file '{_file}'");
			var tokens = new List<Token>();

			_cursor = 0;
			while (!_AtEnd())
			{
				_start = _cursor;
				Token token = _ScanToken();
				if (token != null) 
					tokens.Add(token);
			}

			tokens.Add(new Token { Column = _column, Line = _line, Lexeme = "EOF", Literal = null, Type = TokenType.EOF });
			return tokens;
		}

		private Token _ScanToken()
		{
			char c = _Advance();
			switch (c)
			{
				// Single character tokens
				case '<': return _GenerateToken(TokenType.LEFT_ANGLE_BRACKET, "<");
				case '>': return _GenerateToken(TokenType.RIGHT_ANGLE_BRACKET, "<");
				case '*': return _GenerateToken(TokenType.STAR, "*");
				case '&': return _GenerateToken(TokenType.REF, "&");
				case '(': return _GenerateToken(TokenType.LEFT_PAREN, "(");
				case ')': return _GenerateToken(TokenType.RIGHT_PAREN, ")");
				case '#': return _GenerateToken(TokenType.HASHTAG, "#");
				case '{': return _GenerateToken(TokenType.LEFT_BRACKET, "{");
				case '}': return _GenerateToken(TokenType.RIGHT_BRACKET, "}");
				case ';': return _GenerateToken(TokenType.SEMICOLON, ";");
				case '=': return _GenerateToken(TokenType.EQUAL, "=");
				case '"': return _String();
				// Whitespace
				case '/':
					{
						if (_Match('/'))
						{
							while (_Peek() != '\n' && !_AtEnd())
								_Advance();
							return null;
						}
						else if (_Match('*'))
						{
							while (!_AtEnd() && _Peek() != '*' && _PeekNext() != '/')
							{
								c = _Advance();
								if (c == '\n')
								{
									_column = 0;
									_line++;
								}
							}

							// Consume */
							if (!_AtEnd()) _Match('*');
							if (!_AtEnd()) _Match('/');
							return null;
						}
						break;
					}
				case ' ':
				case '\r':
				case '\t':
					// Ignore whitespace
					return null;
				case '\n':
					_column = 0;
					_line++;
					return null;
				default:
					if (_IsAlpha(c))
					{
						return _PropertyIdentifier();
					}
					break;
			}

			return null;
		}

		private Token _PropertyIdentifier()
		{
			while (_IsAlphaNumeric(_Peek())) _Advance();

			string text = _fileContents.Substring(_start, _cursor - _start);
			TokenType type = TokenType.EOF;
			bool contains = keywords.TryGetValue(text, out type);
			if (contains) return new Token { Lexeme = text, Column = _column, Line = _line, Literal = text, Type = type };
			return new Token { Lexeme = text, Column = _column, Line = _line, Literal = text, Type = TokenType.IDENTIFIER };
		}

		private Token _Number()
		{
			while (_IsDigit(_Peek())) _Advance();

			bool hasE = false;
			if (_Peek() == '.' && (_IsDigit(_PeekNext()) || (_PeekNext() == 'e' && _IsDigit(_PeekNextNext()))
					|| (_PeekNext() == 'E' && _IsDigit(_PeekNextNext()))))
			{
				_Advance();

				while (_IsDigit(_Peek()))
				{
					_Advance();
				}

				if ((_Peek() == 'e' || _Peek() == 'E') && (_IsDigit(_PeekNext()) ||
						((_PeekNext() == '-' && _IsDigit(_PeekNextNext())) || (_PeekNext() == '+' && _IsDigit(_PeekNextNext())))))
				{
					_Advance();
					while (_IsDigit(_Peek())) _Advance();

					if ((_Peek() == '-' || _Peek() == '+') && _IsDigit(_PeekNext()))
					{
						_Advance();
						while (_IsDigit(_Peek())) _Advance();
					}

					if (_Peek() == '.')
					{
						Console.WriteLine($"Unexpected number literal at {_line} col:{_column}");
						return null;
					}
				}
			}

			if ((_Peek() == 'e' || _Peek() == 'E') && (_IsDigit(_PeekNext()) ||
				((_PeekNext() == '-' && _IsDigit(_PeekNextNext())) || (_PeekNext() == '+' && _IsDigit(_PeekNextNext())))))
			{
				_Advance();
				while (_IsDigit(_Peek())) _Advance();

				if ((_Peek() == '-' || _Peek() == '+') && _IsDigit(_PeekNext()))
				{
					_Advance();
					while (_IsDigit(_Peek())) _Advance();
				}

				if (_Peek() == '.')
				{
					Console.WriteLine($"Unexpected number literal at {_line} col:{_column}");
					return null;
				}
			}

			return new Token { Type = TokenType.NUMBER, Column = _column, Lexeme = "number", Line = _line, Literal = Double.Parse(_fileContents.Substring(_start, _cursor - _start)) };
		}

		private Token _String()
		{
			while (_Peek() != '"' && !_AtEnd())
			{
				if (_Peek() == '\n')
				{
					_line++;
					_column = -1;
				}
				_Advance();
			}

			if (_AtEnd())
			{
				Console.WriteLine($"Unexpected string literal at {_line} col:{_column}");
				return null;
			}

			_Advance();

			String value = _fileContents.Substring(_start, _cursor - _start);
			return new Token { Column = _column, Line = _line, Lexeme = value, Literal = value, Type = TokenType.STRING_LITERAL };
		}

		private char _Advance()
		{
			char c = this._fileContents[_cursor];
			_cursor++;
			_column++;
			return c;
		}

		private char _Peek()
		{
			if (_AtEnd()) return '\0';
			return _fileContents[_cursor];
		}

		private char _PeekNext()
		{
			if (_AtEnd() || _cursor == _fileContents.Length - 1) return '\0';
			return _fileContents[_cursor + 1];
		}

		private char _PeekNextNext()
		{
			if (_AtEnd() || _cursor == _fileContents.Length - 1 || _cursor == _fileContents.Length - 2) return '\0';
			return _fileContents[_cursor + 1];
		}

		private bool _IsDigit(char c)
		{
			return c >= '0' && c <= '9';
		}

		private bool _IsAlphaNumeric(char c)
		{
			return _IsAlpha(c) || _IsDigit(c);
		}

		private bool _IsAlpha(char c)
		{
			return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
		}

		private bool _Match(char expected)
		{
			if (_AtEnd()) return false;
			if (_fileContents[_cursor] != expected) return false;

			_cursor++;
			_column++;
			return true;
		}

		private bool _AtEnd()
		{
			return _cursor == _fileContents.Length;
		}

		private Token _GenerateToken(TokenType type, string lexeme)
		{
			return new Token { Line = _line, Column = _column, Lexeme = lexeme, Type = type, Literal = null };
		}
	}
}
