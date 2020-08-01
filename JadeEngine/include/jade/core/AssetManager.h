#pragma once
#include "externalLibs.h"
#include "jade/file/JPath.h"
#include "jade/util/Log.h"

#include <entt/entt.h>

namespace Jade
{
	class NullAsset;
	class Asset
	{
	public:
		Asset() 
			: m_Scene(0), m_ResourceId(0), m_Path(""), m_Loaded(false)
		{
		}

		virtual ~Asset() {}
		virtual void Load() = 0;
		virtual void Unload() = 0;

		uint32 GetResourceId() { return m_ResourceId; }
		const JPath& GetPath() { return m_Path; }
		uint32 GetType() { return m_Type; }
		void SetResourceId(uint32 id) { m_ResourceId = id; }
		bool IsNull() { return GetType() == Asset::GetResourceTypeId<NullAsset>(); }

		template<class T>
		void GenerateTypeId()
		{
			m_Type = (uint32)entt::type_info<T>().id();
		}

		template<typename T>
		static uint32 GetResourceTypeId()
		{
			return (uint32)entt::type_info<T>().id();
		}

	protected:
		uint32 m_Type = 0;
		uint32 m_Scene = 0;
		uint32 m_ResourceId = 0;
		JPath m_Path;
		bool m_Loaded;
	};

	class AssetManager
	{
	public:
		static std::unordered_map<uint32, std::shared_ptr<Asset>>& GetAllAssets(uint32 scene);
		static std::shared_ptr<Asset> GetAsset(uint32 resourceID);
		static std::shared_ptr<Asset> GetAsset(const JPath& path);
		static std::shared_ptr<Asset> LoadTextureFromFile(const JPath& path);

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

		static void Clear();
		static void Init(uint32 scene);

		static uint32 GetScene() { return Get()->m_CurrentScene; }
		static void SetScene(uint32 scene) { Get()->m_CurrentScene = scene; }

	private:
		AssetManager(int scene)
			: m_CurrentScene(scene), m_ResourceCount(0)
		{
		}

		static AssetManager* Get();
		static std::unique_ptr<AssetManager> s_Instance;

	protected:
		uint32 m_CurrentScene;
		uint32 m_ResourceCount;

		// Map from scene -> AssetID -> Asset
		std::unordered_map<uint32, std::unordered_map<uint32, std::shared_ptr<Asset>>> m_Assets;
	};

	class NullAsset : public Asset
	{
	public:
		NullAsset()
		{
			GenerateTypeId<NullAsset>();
		}

		virtual void Load() override
		{
			Log::Warning("Attempting to load a null asset.");
		}

		virtual void Unload() override
		{
			Log::Warning("Attempting to unload a null asset.");
		}
	};
}