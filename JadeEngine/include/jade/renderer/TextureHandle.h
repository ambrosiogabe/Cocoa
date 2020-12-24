#pragma once
#include "jade/core/Core.h"
#include "externalLibs.h"

#include "jade/renderer/Texture.h"
#include "jade/core/AssetManager.h"

namespace Jade
{
	class JADE TextureHandle
	{
	public:
		TextureHandle()
		{
			m_AssetId = -1;
		}

		TextureHandle(uint32 id)
		{
			m_AssetId = id;
		}

		TextureHandle(const TextureHandle& other)
		{
			m_AssetId = other.m_AssetId;
		}


		inline std::shared_ptr<Texture> Get() const
		{
			return std::static_pointer_cast<Texture>(AssetManager::GetAsset(m_AssetId));
		}

		inline bool operator==(TextureHandle other) const
		{
			return m_AssetId == other.m_AssetId;
		}

		inline bool operator!=(TextureHandle other) const
		{
			return !(*this == other);
		}

		inline operator bool() const
		{
			return *this != TextureHandle::null;
		}

	public:
		static TextureHandle null;
		uint32 m_AssetId;
	};
}