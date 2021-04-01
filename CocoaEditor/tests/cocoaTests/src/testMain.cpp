#include "externalLibs.h"
#include "cocoa/core/Core.h"
#include "cocoa/util/Log.h"

#include "cocoaTests/include/testMain.h"
#include "cocoaTests/include/ScriptParserTests.h"

#ifdef COCOA_TEST

namespace Cocoa
{
	namespace Tests
	{
		void RunTests()
		{
			ScriptParserTests::TestTokenScanning();
			Log::Info("TestTokenScanning Passed.");
		}
	}
}

#endif