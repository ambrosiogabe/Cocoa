#pragma once

#define Log(format, ...) _Log(__FILE__, __LINE__, format, __VA_ARGS__)
#define Info(format, ...) _Info(__FILE__, __LINE__, format, __VA_ARGS__)
#define Warning(format, ...) _Warning(__FILE__, __LINE__, format, __VA_ARGS__)
#define Error(format, ...) _Error(__FILE__, __LINE__, format, __VA_ARGS__)
#define Assert(condition, format, ...) _Assert(__FILE__, __LINE__, condition, format, __VA_ARGS__)

namespace Cocoa
{
	namespace Log
	{
		COCOA void _Log(const char* filename, int line, const char* format, ...);
		COCOA void _Info(const char* filename, int line, const char* format, ...);
		COCOA void _Warning(const char* filename, int line, const char* format, ...);
		COCOA void _Error(const char* filename, int line, const char* format, ...);
		COCOA void _Assert(const char* filename, int line, int condition, const char* format, ...);
	};
}