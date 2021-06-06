#ifndef COCOA_EDITOR_CPP_BUILD_H
#define COCOA_EDITOR_CPP_BUILD_H
#include "cocoa/file/Path.h"

namespace Cocoa
{
	namespace CppBuild
	{
		void build(const Path& projectDirectory);

		void compile(const Path& projectDirectory);
	}
}

#endif
