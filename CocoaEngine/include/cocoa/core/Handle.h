#pragma once
#include "cocoa/core/Core.h"
#include "externalLibs.h"

namespace Cocoa
{
	namespace NHandle
	{
		const uint32 NULL_ID = UINT32_MAX;
	}

	template<typename T>
	struct Handle
	{
		uint32 AssetId;

		inline bool operator==(Handle other) const
		{
			return AssetId == other.AssetId;
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
			return AssetId == NHandle::NULL_ID;
		}
	};

	namespace NHandle
	{
		template<typename T>
		Handle<T> CreateHandle()
		{
			return Handle<T> {
				NULL_ID
			};
		}

		template<typename T>
		Handle<T> CreateHandle(uint32 id)
		{
			return Handle<T> {
				id
			};
		}
	}
}