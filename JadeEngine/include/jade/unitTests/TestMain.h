#pragma once

#include "TestFactory.h"
#include "CollisionDetector2DTester.h"

namespace Jade
{
	class TestMain
	{
	public:
		static bool Test();
	};

	bool TestMain::Test()
	{
		for (auto it = TestFactory::Tests.begin(); it != TestFactory::Tests.end(); it++)
		{
			if (!it->second())
			{
				Log::Error("Test failed: '%s'", it->first.c_str());
				return false;
			}
		}
		return true;
	}
}