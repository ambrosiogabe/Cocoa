#pragma once
#include "externalLibs.h"
#include "cocoa/file/CPath.h"
#include "cocoa/util/Log.h"
#include "cocoa/renderer/fonts/Font.h"
#include "cocoa/renderer/Texture.h"
#include "cocoa/core/Handle.h"

#include <entt/entt.hpp>

namespace Cocoa
{
	enum class AssetType : char
	{
		None=0,
		Texture=1,
		Font=2
	};

	class COCOA AssetManager
	{
	public:		
		template<typename T>
		static std::vector<std::shared_ptr<T>> GetAllAssets(uint32 scene)
		{
			AssetManager* manager = Get();
			auto it = manager->m_Assets.find(scene);
			if (it == manager->m_Assets.end())
			{
				return std::vector<std::shared_ptr<T>>{};
			}

			std::vector<std::shared_ptr<T>> res{};
			for (auto assetIt = it->second.begin(); assetIt != it->second.end(); assetIt++)
			{
				if (assetIt->second->GetType() == Asset::GetResourceTypeId<T>())
				{
					res.push_back(std::static_pointer_cast<T>(assetIt->second));
				}
			}

			return res;
		}

		static Handle<Texture> LoadTextureFromFile(const CPath& path, bool isDefault=false);
		static Handle<Texture> GetTexture(const CPath& path);

		static const Texture& GetTexture(uint32 resourceId);
		static const Font& GetFont(uint32 resourceId);

		static std::unordered_map<uint32, uint32> LoadFrom(const json& j);
		static json Serialize();

		static void Clear();
		static void Init(uint32 scene);

		static const std::vector<Texture>& GetAllTextures() { return s_Textures; }

	public:
		static uint32 s_CurrentScene;
		static uint32 s_ResourceCount;

		static std::vector<Texture> s_Textures;
		static std::vector<Font> s_Fonts;
	};
}