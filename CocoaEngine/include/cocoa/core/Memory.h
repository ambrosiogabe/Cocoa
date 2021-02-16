#pragma once
#include "cocoa/core/Core.h"

#define AllocMem(numBytes) Cocoa::Memory::_Allocate(__FILE__, __LINE__, numBytes)
#define ReallocMem(memory, newSize) Cocoa::Memory::_Realloc(__FILE__, __LINE__, memory, newSize)
#define FreeMem(memory) Cocoa::Memory::_Free(__FILE__, __LINE__, memory)

namespace Cocoa
{
	namespace Memory
	{
		COCOA void* _Allocate(const char* filename, int line, size_t numBytes);
		COCOA void* _Realloc(const char* filename, int line, void* memory, size_t newSize);
		COCOA void _Free(const char* filename, int line, void* memory);
		COCOA void Init();
		COCOA void Destroy();
	}
}