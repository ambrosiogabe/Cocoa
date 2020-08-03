#include "jade/core/AssetManager.h"
#include "jade/util/Log.h"
#include "jade/renderer/Texture.h"
#include "jade/file/IFile.h"

namespace Jade
{
	std::unique_ptr<AssetManager> AssetManager::s_Instance = nullptr;
	AssetManager* AssetManager::Get()
	{
		Log::Assert(s_Instance != nullptr, "Asset Manager never initialized.");
		return s_Instance.get();
	}

	void AssetManager::Init(uint32 scene)
	{
		Log::Assert(s_Instance == nullptr, "Asset Manager is already initialized. Cannot initailze twice.");
		s_Instance = std::unique_ptr<AssetManager>(new AssetManager(scene));
	}

	std::unordered_map<uint32, std::shared_ptr<Asset>>& AssetManager::GetAllAssets(uint32 scene)
	{
		AssetManager* manager = Get();
		auto it = manager->m_Assets.find(scene);
		if (it == manager->m_Assets.end())
		{
			return manager->m_EmptyAssetContainer;
		}
		return it->second;
	}

	std::shared_ptr<Asset> AssetManager::GetAsset(uint32 assetId)
	{
		AssetManager* manager = Get();
		auto assetsIt = manager->m_Assets.find(manager->m_CurrentScene);
		if (assetsIt != manager->m_Assets.end())
		{
			auto assetIt = assetsIt->second.find(assetId);
			if (assetIt != assetsIt->second.end())
			{
				return assetIt->second;
			}
		}

		return std::shared_ptr<Asset>(new NullAsset());
	}

	std::shared_ptr<Asset> AssetManager::GetAsset(const JPath& path)
	{
		AssetManager* manager = Get();
		auto assetsIt = manager->m_Assets.find(manager->m_CurrentScene);
		if (assetsIt != manager->m_Assets.end())
		{
			for (auto asset = assetsIt->second.begin(); asset != assetsIt->second.end(); asset++)
			{
				if (asset->second->GetPath() == path)
				{
					return asset->second;
				}
			}
		}

		return std::shared_ptr<Asset>(new NullAsset());
	}

	std::shared_ptr<Asset> AssetManager::LoadTextureFromFile(const JPath& path)
	{
		AssetManager* manager = Get();

		std::shared_ptr<Asset> assetExists = GetAsset(path);
		if (!assetExists->IsNull())
		{
			Log::Warning("Tried to load asset that has already been loaded.");
			return std::shared_ptr<Asset>(new NullAsset());
		}

		JPath absPath = IFile::GetAbsolutePath(path);
		std::shared_ptr<Texture> newAsset = std::make_shared<Texture>(absPath.Filepath());

		auto& assets = manager->m_Assets[manager->m_CurrentScene];

		uint32 newId = (uint32)assets.size();
		newAsset->SetResourceId(newId);
		assets.insert({ newId, newAsset });
		newAsset->Load();
		return newAsset;
	}

	void AssetManager::Clear()
	{
		AssetManager* manager = Get();
		for (auto assetListIt = manager->m_Assets.begin(); assetListIt != manager->m_Assets.end(); assetListIt++)
		{
			for (auto assetIt = assetListIt->second.begin(); assetIt != assetListIt->second.end(); assetIt++)
			{
				auto asset = assetIt->second;
				if (!asset->IsNull())
				{
					asset->Unload();
				}
			}
			assetListIt->second.clear();
		}

		manager->m_Assets.clear();
	}

	void AssetManager::LoadFrom(const json& j)
	{
		Clear();
		AssetManager* manager = Get();

		// TODO: SERIALIZE SCENE ID WITH EACH SCENE, AND ASSET ID WITH EACH ASSET
		// TODO: ALSO CREATE DESERIALIZE METHOD FOR ASSET
	}

	json AssetManager::Serialize()
	{
		json res;

		AssetManager* manager = Get();

		res["SceneID"] = manager->m_CurrentScene;
		auto assetList = manager->m_Assets[manager->m_CurrentScene];
		res["AssetCount"] = assetList.size();
		

		for (auto assetIt = assetList.begin(); assetIt != assetList.end(); assetIt++)
		{
			json assetSerialized = assetIt->second->Serialize();
			if (assetSerialized["Type"] != 0)
			{
				res["AssetList"][std::to_string(assetIt->first)] = assetSerialized;
			}
		}

		return res;
	}

	json Asset::Serialize()
	{
		Asset::AssetType type = Asset::AssetType::None;
		if (GetType() == Asset::GetResourceTypeId<Texture>())
		{
			type = Asset::AssetType::Texture;
		}
		else
		{
			return {{"Type", Asset::AssetType::None}};
		}

		return {
			{"Type", type},
			{"Filepath", m_Path.Filepath()},
			{"ResourceId", m_ResourceId}
		};
	}
}