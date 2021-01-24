#pragma once
#include "cocoa/core/Core.h"
#include "externalLibs.h"

namespace Cocoa
{
	class AssetManager;

	template<typename T>
	class COCOA Handle
	{
	public:
		Handle()
		{
			m_Type = entt::type_info<T>().id();
			m_AssetId = m_NullId;
		}

		Handle(uint32 id)
		{
			m_Type = entt::type_info<T>().id();
			m_AssetId = id;
		}

		Handle(const Handle& other)
		{
			m_Type = entt::type_info<T>().id();
			m_AssetId = other.m_AssetId;
		}

		inline bool operator==(Handle other) const
		{
			return m_AssetId == other.m_AssetId;
		}

		inline bool operator!=(Handle other) const
		{
			return !(*this == other);
		}

		inline operator bool() const
		{
			return !IsNull();
		}

		inline bool IsNull() const
		{
			return m_AssetId == m_NullId;
		}

	public:
		uint32 m_AssetId;
		uint32 m_Type;

	private:
		uint32 m_NullId = -1;
	};
}