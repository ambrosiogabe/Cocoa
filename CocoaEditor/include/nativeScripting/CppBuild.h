#ifndef COCOA_EDITOR_CPP_BUILD_H
#define COCOA_EDITOR_CPP_BUILD_H
#include <filesystem>

namespace Cocoa
{
	namespace CppBuild
	{
		void build(const std::filesystem::path& projectDirectory);

		void compile(const std::filesystem::path& projectDirectory);
	}
}

#endif
