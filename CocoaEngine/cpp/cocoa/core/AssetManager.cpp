#include "cocoa/core/AssetManager.h"
#include "cocoa/util/Log.h"
#include "cocoa/renderer/Texture.h"
#include "cocoa/file/IFile.h"
#include "cocoa/util/JsonExtended.h"

namespace Cocoa
{
	std::vector<Texture> AssetManager::s_Textures = std::vector<Texture>();
	std::vector<Font> AssetManager::s_Fonts = std::vector<Font>();
	uint32 AssetManager::s_CurrentScene = 0;
	uint32 AssetManager::s_ResourceCount = 0;

	void AssetManager::Init(uint32 scene)
	{
		s_CurrentScene = scene;
		s_ResourceCount = 0;
		// TODO: Clear assets somehow?
	}

	const Texture& AssetManager::GetTexture(uint32 resourceId)
	{
		if (resourceId < s_Textures.size())
		{
			return s_Textures[resourceId];
		}

		return Texture::nullTexture;
	}

	const Font& AssetManager::GetFont(uint32 resourceId)
	{
		if (resourceId < s_Fonts.size())
		{
			return s_Fonts[resourceId];
		}

		return Font::nullFont;
	}

	Handle<Texture> AssetManager::GetTexture(const CPath& path)
	{
		int i = 0;
		for (auto& tex : s_Textures)
		{
			if (tex.GetFilepath() == path)
			{
				return Handle<Texture>(i);
			}
			i++;
		}

		return Handle<Texture>();
	}

	Handle<Texture> AssetManager::LoadTextureFromFile(const CPath& path, bool isDefault)
	{
		Handle<Texture> texture = GetTexture(path);
		if (!texture.IsNull())
		{
			Log::Warning("Tried to load asset that has already been loaded.");
			return texture;
		}

		CPath absPath = IFile::GetAbsolutePath(path);
		std::shared_ptr<Texture> newAsset = std::make_shared<Texture>(absPath.Filepath(), isDefault);
		s_Textures.push_back({ absPath.Filepath(), isDefault });

		int index = s_Textures.size() - 1;
		Texture& newTexture = s_Textures.at(index);
		newTexture.Load();
		return Handle<Texture>(index);
	}

	json AssetManager::Serialize()
	{
		json res;

		res["SceneID"] = s_CurrentScene;
		int assetCount = 0;

		int i = 0;
		for (auto& assetIt : s_Textures)
		{
			if (!assetIt.IsDefault())
			{
				json assetSerialized = assetIt.Serialize();
				assetSerialized["ResourceId"] = i;
				res["AssetList"][std::to_string(i)] = assetSerialized;
				i++;
				assetCount++;
			}
		}

		for (auto& assetIt : s_Fonts)
		{
			//if (!assetIt.IsDefault())
			//{
			//	json assetSerialized = assetIt.Serialize();
			//	assetSerialized["ResourceId"] = i;
			//	i++;
			//	assetCount++;
			//}
		}

		res["AssetCount"] = assetCount;

		return res;
	}

	std::unordered_map<uint32, uint32> AssetManager::LoadFrom(const json& j)
	{
		std::unordered_map<uint32, uint32> resourceIDMap{};

		uint32 scene = -1;
		JsonExtended::AssignIfNotNull(j["SceneID"], scene);
		int assetCount = -1;
		JsonExtended::AssignIfNotNull(j["AssetCount"], assetCount);

		if (scene >= 0 && assetCount > 0)
		{
			for (auto it = j["AssetList"].begin(); it != j["AssetList"].end(); ++it)
			{
				const json& assetJson = it.value();
				uint32 type = 0;
				JsonExtended::AssignIfNotNull(assetJson["Type"], type);
				uint32 resourceId = -1;
				JsonExtended::AssignIfNotNull(assetJson["ResourceId"], resourceId);

				CPath path = "";
				if (!assetJson["Filepath"].is_null())
				{
					path = CPath(assetJson["Filepath"], false);
				}

				if (resourceId >= 0)
				{
					switch (static_cast<AssetType>(type))
					{
					case AssetType::None:
						Log::Warning("Tried to deserialize asset of type none: %s", path.Filepath());
						break;
					case AssetType::Texture:
					{
						Handle<Texture> tex = LoadTextureFromFile(path);
						resourceIDMap.insert({ resourceId, tex.m_AssetId });
					}
					break;
					default:
						Log::Warning("Unkown asset type %d for: %s", type, path.Filepath());
						break;
					}
				}
			}
		}

		return resourceIDMap;
	}

	void AssetManager::Clear()
	{
		s_Textures.clear();
		s_Fonts.clear();
	}
}