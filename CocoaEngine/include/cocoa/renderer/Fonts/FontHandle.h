#pragma once
#include "cocoa/core/Core.h"
#include "externalLibs.h"

#include "cocoa/renderer/fonts/Font.h"
#include "cocoa/core/AssetManager.h"

namespace Cocoa
{
	class COCOA FontHandle
	{
	public:
		FontHandle()
		{
			m_AssetId = -1;
		}

		FontHandle(uint32 id)
		{
			m_AssetId = id;
		}

		FontHandle(const TextureHandle& other)
		{
			m_AssetId = other.m_AssetId;
		}


		inline std::shared_ptr<Font> Get() const
		{
			return std::static_pointer_cast<Font>(AssetManager::GetAsset(m_AssetId));
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
			return *this != FontHandle::null;
		}

	public:
		static FontHandle null;
		uint32 m_AssetId;
	};
}