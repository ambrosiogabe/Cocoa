#include "jade/core/AssetManager.h"

namespace Jade
{
	std::unordered_map<std::string, Texture*> AssetManager::s_Textures{};

	void AssetManager::ImportSpritesheet(const std::string& filename, Spritesheet* spritesheet)
	{
		//if (m_Textures.find(filename) == m_Assets.end())
		//{
		//	m_Assets.emplace(filename, AssetHandle{AssetType::Spritesheet, (void*)spritesheet});
		//}
		//else
		//{
		//	Log::Warning("Importing spritesheet that already exists. %s", filename.c_str());
		//}
	}

	void AssetManager::ImportSprite(const std::string& filename)
	{
		if (s_Textures.find(filename) == s_Textures.end())
		{
			Texture* texture = new Texture(filename.c_str());
			s_Textures.emplace(filename, texture);
		}
		else
		{
			Log::Warning("Importing sprite with texture that already exists. %d", filename.c_str());
		}
	}

	Texture* AssetManager::GetTexture(const std::string& filename)
	{
		bool has = s_Textures.find(filename) != s_Textures.end();
		if (!has)
		{
			Log::Warning("Requesting uninitialized texture: %s", filename.c_str());
		}
		return has ? s_Textures.at(filename) : nullptr;
	}
}