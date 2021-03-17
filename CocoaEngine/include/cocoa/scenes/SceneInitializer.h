#pragma once
#include "cocoa/core/Core.h"
#include "externalLibs.h"

#include "cocoa/scenes/SceneData.h"

namespace Cocoa
{
	class COCOA SceneInitializer
	{
	public:
		SceneInitializer() {}

		virtual void Init(SceneData& scene) = 0;
		virtual void Start(SceneData& scene) = 0;
		virtual void Destroy(SceneData& scene) = 0;
		virtual void Save(SceneData& scene) = 0;
		virtual void Load(SceneData& scene) = 0;
	};
}