#pragma once
#include "cocoa/file/CPath.h"

namespace Cocoa
{
	namespace CppBuild
	{
		void Build(const CPath& projectDirectory);

		void Compile(const CPath& projectDirectory);
	}
}