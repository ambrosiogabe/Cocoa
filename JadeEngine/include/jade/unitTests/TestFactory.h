#pragma once
#include "externalLibs.h"

// UNIT TEST MACRO
#define JADE_TEST(name) \
			bool test_##name(); \
			static bool test_##name##_registered = TestFactory::Register(#name, &test_##name); \
			bool test_##name()

namespace Jade
{
	class TestFactory
	{
	public:
		friend class TestMain;
		static bool Register(std::string name, std::function<bool()> func);
	private:
		static std::unordered_map<std::string, std::function<bool()>> Tests;
	};

	std::unordered_map<std::string, std::function<bool()>> TestFactory::Tests;

	bool TestFactory::Register(std::string name, std::function<bool()> func)
	{
		auto it = Tests.find(name);
		if (it == Tests.end())
		{
			Tests[name] = std::move(func);
			return true;
		}
		return false;
	}
}