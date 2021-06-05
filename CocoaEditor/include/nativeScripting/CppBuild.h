#ifndef COCOA_EDITOR_CPP_BUILD_H
#define COCOA_EDITOR_CPP_BUILD_H
#include "cocoa/file/CPath.h"

namespace Cocoa
{
	namespace CppBuild
	{
		void build(const CPath& projectDirectory);

		void compile(const CPath& projectDirectory);
	}
}

#endif
