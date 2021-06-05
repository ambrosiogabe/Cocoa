#ifndef COCOA_ENGINE_RENDER_SYSTEM_H
#define COCOA_ENGINE_RENDER_SYSTEM_H
#include "externalLibs.h"
#include "cocoa/core/Core.h"

#include "cocoa/components/SpriteRenderer.h"
#include "cocoa/components/FontRenderer.h"
#include "cocoa/components/Transform.h"
#include "cocoa/scenes/SceneData.h"

namespace Cocoa
{
	namespace RenderSystem
	{
		COCOA void init();
		COCOA void destroy();

		COCOA void addEntity(const TransformData& transform, const FontRenderer& fontRenderer);
		COCOA void addEntity(const TransformData& transform, const SpriteRenderer& spr);
		COCOA void render(const SceneData& scene);

		COCOA void serialize(json& j, Entity entity, const SpriteRenderer& spriteRenderer);
		COCOA void deserializeSpriteRenderer(const json& json, Entity entity);
		COCOA void serialize(json& j, Entity entity, const FontRenderer& fontRenderer);
		COCOA void deserializeFontRenderer(const json& json, Entity entity);
	};
}

#endif
