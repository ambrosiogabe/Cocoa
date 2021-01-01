#pragma once
#include "cocoa/core/AssetManager.h"

namespace Jade
{
	class COCOA Audio : public Asset
	{
	public:
		Audio()
		{
			GenerateTypeId<Audio>();
		}
	};
}