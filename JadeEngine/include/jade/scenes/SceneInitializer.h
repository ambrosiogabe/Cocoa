#pragma once
#include "jade/core/Core.h"
#include "externalLibs.h"

#include "jade/systems/System.h"

namespace Jade
{
	class Scene;
	class JADE SceneInitializer
	{
	public:
		SceneInitializer() {}

		virtual void Init(Scene* scene, std::vector<std::unique_ptr<System>>& systems) = 0;
	};
}