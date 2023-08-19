#ifndef COCOA_ENGINE_HANDLE_H
#define COCOA_ENGINE_HANDLE_H
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
		uint32 assetId;

		bool operator==(Handle other) const
		{
			return assetId == other.assetId;
		}

		bool operator!=(Handle other) const
		{
			return !(*this == other);
		}

		operator bool() const
		{
			return !isNull();
		}

		bool isNull() const
		{
			return assetId == NHandle::NULL_ID;
		}
	};

	namespace NHandle
	{
		template<typename T>
		Handle<T> createHandle()
		{
			return Handle<T> {
				NULL_ID
			};
		}

		template<typename T>
		Handle<T> createHandle(uint32 id)
		{
			return Handle<T> {
				id
			};
		}
	}
}

#endif
