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

	class AssetManager
	{
	public:
		static void ImportSpritesheet(const std::string& filename, Spritesheet* spritesheet);
		static void ImportSprite(const std::string& filename);

		static Texture* GetTexture(const std::string& filename);

	private:
		static std::unordered_map<std::string, Texture*> s_Textures;
	};
}