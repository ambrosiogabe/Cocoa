#pragma once
#include "jade/core/AssetManager.h"

namespace Jade
{
	class JADE Audio : public Asset
	{
	public:
		Audio()
		{
			GenerateTypeId<Audio>();
		}
	};
}