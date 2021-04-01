#if false
// This is a file for testing my scanner
/*
	I will try to include every valid scanning possibility
	and ensure that the scanner returns the tokens exactly as they
	should be.

	Boy what an annoying test suite to write.
*/

namespace MyNameSpace
{
	namespace AnotherNamespace
	{

	}
}

template<typename Foo>
void TestAngleBracketTokens()
{
	int a = 1 << 10;
	a <<= 5;
	a >>= 5;
	a = 1 >> 10;
	int b = 0;
	a < b;
	a > b;
	a <= b;
	a >= b;
}

void TestDecimalNumberLiterals()
{
	// Octal numbers. Start with a 0, only contain digits 0-7
	int a = 0173;
	int b = 01234567;

	// Decimal integers, Start with non-0 number and only contain digits 0-9
	// You can use ' character to separate digits to make them more readable
	int c = 103;
	int d = 1234567890;
	int e = 1'234'567'890;

	// Hexadecimal literals, begin with '0x' or '0X'
	// Can contain digits 0-9 and letters 'a'-'f' or 'A'-'F' case does not matter
	int g = 0x0123456789abcdef;
	int h = 0X0123456789ABCDEF;

	// Unsigned literal ends in 'u' or 'U'
	// Octal/Hex literals can also be denoted as unsigned
	unsigned i = 238u;
	unsigned j = 238U;
	unsigned k = 0x238U;
	unsigned l = 0237U;

	// Long literal ends in 'L' or 'l'
	// Octal/Hex literals can also be denoted as long
	long m = 238l;
	long n = 238L;
	long o = 0x238L;
	long p = 0237L;

	// Unsigned long ends in 'ul' or 'UL' or 'uL' or 'Ul'
	// Octal/Hex literals can also be denoted as unsigned long
	unsigned long q = 238ul;
	unsigned long r = 238Ul;
	unsigned long s = 238uL;
	unsigned long t = 238UL;
	unsigned long u = 0x238ul;
	unsigned long v = 0237ul;

	// Signed long long ends in 'll' or 'LL' or 'lL' or 'Ll'
	// Octal/Hex literals can also be denoted as long long
	long long w = 238ll;
	long long x = 238LL;
	long long y = 238lL;
	long long z = 238Ll;
	long long A = 0x23all;
	long long B = 0237ll;

	// I'm just not going to test unsigned long for all cases, but it should work 
	// for any variant of 'ull' mixed case
	// Octal/Hex literals can also be denoted as unsigned long long
	unsigned long long C = 234ull;
	unsigned long long D = 0x23aULL;
	auto E = 0237ULL;
}

void TestFloatingPointNumberLiterals()
{
	// Doubles are denoted without an 'f' at the end
	// Doubles can also end with a '.'
	double a = 18.46;
	double b = 18.;

	// Long doubles end in 'l' or 'L'
	double la = 18.46l;
	double lb = 18.46L;

	// Scientific notation is supported for doubles and floats
	// It use 'e' or 'E'
	double c = 18.46e10;
	double d = 18.46E-01;
	double e = 18.46E+1;
	float f = 18.46e0f;
	float g = 18.46e-20f;
	float h = 18.46e+20f;
	float i = .3e10;
	float j = 0.3e10;
	float k = .3f;
	float l = 2.e10f;
}

void TestBinaryLiterals()
{
	// Binary literals start with '0b' or '0B' followed by 0's and 1's
	int a = 0b11010101;
	int b = 0B0101110001;
}

void TestCharacterLiterals()
{
	// Character literals
	auto c0 = 'A'; // char
	auto c1 = u8'A'; // char
	auto c2 = L'A'; // wchar_t
	auto c3 = u'A'; // char16_t
	auto c4 = U'A'; // char32_t

	// Escaped character literals
	auto e0 = '\n';
	auto e1 = '\\';
	auto e2 = '\t';
	auto e3 = '\?';
	auto e4 = '\v';
	auto e5 = '\'';
	auto e6 = '\b';
	auto e7 = '\"';
	auto e8 = '\r';
	auto e9 = '\0';
	auto e10 = '\f';
	auto e11 = '\023';
	auto e12 = '\a';
	auto e13 = '\x0ab';
	auto e14 = '\U00000041'; // Unicode is u followed by 8 digits

	// Multicharacter literals
	auto m0 = 'abcd'; // int, value 0x61626364
}

void TestStringLiterals()
{
	// String literals
	auto s0 = "hello"; // const char*
	auto s1 = u8"hello"; // const char*, encoded as UTF-8
	auto s2 = L"hello"; // const wchar_t*
	auto s3 = u"hello"; // const char16_t*, encoded as UTF-16
	auto s4 = U"hello"; // const char32_t*, encoded as UTF-32

	// Raw string literals containing unescaped \ and "
	auto R0 = R"("Hello \ world")"; // const char*
	auto R1 = u8R"("Hello \ world")"; // const char*, encoded as UTF-8
	auto R2 = LR"("Hello \ world")"; // const wchar_t*
	auto R3 = uR"("Hello \ world")"; // const char16_t*, encoded as UTF-16
	auto R4 = UR"("Hello \ world")"; // const char32_t*, encoded as UTF-32

	const char* validString = "Hello \
								World";
	const char* validString2 = "Hello" "World";

	const char* validRawLiteral = R"(
Hello world
My name is gabe and this is a very large valid literal
end)";

	const char* otherValidRawLiteral = R"xyz(
This is a custom string literal with a specific 
identifier. That way I can use () parentheses and not have
to worry about it breaking the raw literalness of this string.
)xyz";
}
#endif