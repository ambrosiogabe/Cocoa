#pragma once
#include "jade/core/AssetManager.h"

namespace Jade
{
	class Audio : public Asset
	{
	public:
		Audio()
		{
			GenerateTypeId<Audio>();
		}
	};
}