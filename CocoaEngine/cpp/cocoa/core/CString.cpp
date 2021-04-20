#include "cocoa/core/CString.h"
#include "cocoa/core/Memory.h"
#include "cocoa/util/Log.h"

namespace Cocoa
{
	namespace NCString
	{
		CString Create(std::string& strToCopy)
		{
			CString cstring;
			// TODO: Is this cast safe?
			cstring.Length = (int)strToCopy.length();
			cstring.DynamicAllocation = true;
			cstring.String = (char*)AllocMem(sizeof(char) * cstring.Length + 1);
			for (int i = 0; i < cstring.Length; i++)
			{
				cstring.String[i] = strToCopy[i];
			}

			cstring.String[cstring.Length] = '\0';
			return cstring;
		}

		CString Create(const char* readonlyString)
		{
			CString cstring;
			cstring.Length = strlen(readonlyString);
			cstring.DynamicAllocation = false;
			// TODO: Maybe not cast away the constness?
			cstring.String = (char*)readonlyString;
			return cstring;
		}

		void Free(CString& cstring)
		{
			if (cstring.DynamicAllocation)
			{
				Log::Assert(cstring.String != nullptr, "Tried to free invalid cstring.");
				FreeMem(cstring.String);
				cstring.String = nullptr;
				cstring.Length = 0;
				cstring.DynamicAllocation = false;
			}
		}
	}
}