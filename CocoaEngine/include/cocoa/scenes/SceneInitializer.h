#pragma once
#include "cocoa/core/Core.h"
#include "externalLibs.h"

namespace Cocoa
{
	class Scene;
	class COCOA SceneInitializer
	{
	public:
		SceneInitializer() {}

		virtual void Init(Scene* scene) = 0;
		virtual void Start(Scene* scene) = 0;
	};
}