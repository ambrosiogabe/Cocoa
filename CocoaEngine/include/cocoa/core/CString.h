#pragma once
#include "externalLibs.h"
#include "cocoa/core/Core.h"

namespace Cocoa
{
	struct CString
	{
		char* String;
		int Length;
		bool DynamicAllocation;
	};

	namespace NCString
	{
		COCOA CString Create(std::string& strToCopy);
		COCOA CString Create(const char* readonlyString);
		COCOA void Free(CString& cstring);
	}
}
