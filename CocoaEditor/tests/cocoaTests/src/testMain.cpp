#include "externalLibs.h"
#include "cocoa/core/Core.h"
#include "cocoa/util/Log.h"
#include "cocoa/util/Settings.h"

#include "cocoaTests/include/testMain.h"
#include "cocoaTests/include/ScriptParserTests.h"

#include "nativeScripting/ScriptScanner.h"
#include "nativeScripting/ScriptParser.h"
#include "util/Settings.h"

#ifdef COCOA_TEST

namespace Cocoa
{
	namespace Tests
	{
		void RunTests()
		{
			ScriptParserTests::TestTokenScanning();
			Log::Info("TestTokenScanning Passed.");

			CPath testFile = Settings::General::s_EngineSourceDirectory;
			NCPath::Join(testFile, NCPath::CreatePath("CocoaEditor/tests/cocoaTests/src/ParsingTestFile.cpp"));
			ScriptScanner scanner = ScriptScanner(testFile);
			std::vector<Token> tokens = scanner.ScanTokens();
			ScriptParser parser = ScriptParser(tokens, testFile);
			parser.Parse();
		}
	}
}

#endif