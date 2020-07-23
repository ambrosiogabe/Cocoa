#include "jade/core/AssetManager.h"

namespace Jade
{
	void Assets::ImportSpritesheet(const std::string& filename, Spritesheet* spritesheet)
	{
		if (m_Assets.find(filename) == m_Assets.end())
		{
			m_Assets.emplace(filename, AssetHandle{AssetType::Spritesheet, (void*)spritesheet});
		}
		else
		{
			Log::Warning("Importing spritesheet that already exists. %s", filename.c_str());
		}
	}

	void Assets::ImportSprite(const std::string& filename)
	{

	}
}