#pragma once
#include "cocoa/core/Core.h"
#include "externalLibs.h"

#include "cocoa/systems/System.h"

namespace Cocoa
{
	class Scene;
	class COCOA SceneInitializer
	{
	public:
		SceneInitializer() {}

		virtual void Init(Scene* scene, std::vector<std::unique_ptr<System>>& systems) = 0;
		virtual void Start(Scene* scene) = 0;
	};
}