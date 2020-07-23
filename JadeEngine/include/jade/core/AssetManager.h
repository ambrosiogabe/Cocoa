#pragma once
#include "externalLibs.h"
#include "jade/components/components.h"

namespace Jade
{
	enum class AssetType
	{
		None,
		Sprite,
		Spritesheet
	};

	struct AssetHandle
	{
		AssetType type;
		void* data;
	};

	class Assets
	{
	public:
		void ImportSpritesheet(const std::string& filename, Spritesheet* spritesheet);
		void ImportSprite(const std::string& filename);

	private:
		std::unordered_map<std::string, AssetHandle> m_Assets;
	};
}