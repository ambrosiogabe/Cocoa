#pragma once
#include "externalLibs.h"
#include "cocoa/util/Log.h"
#include "cocoa/core/Memory.h"

namespace Cocoa
{
	template<typename T>
	struct DynamicArray
	{
		T* m_Data;
		int m_NumElements;
		int m_MaxSize;
	};

	namespace NDynamicArrayPrivate
	{
		template<typename T>
		void CheckResize(DynamicArray<T>& data, int numElementsToAdd)
		{
			if (data.m_NumElements + numElementsToAdd > data.m_MaxSize)
			{
				data.m_MaxSize = (data.m_NumElements + numElementsToAdd) * 2;
				data.m_Data = (T*)ReallocMem(data.m_Data, sizeof(T) * data.m_MaxSize);
			}
		}
	}

	namespace NDynamicArray
	{
		template<typename T>
		DynamicArray<T> Create(int size = 0)
		{
			DynamicArray<T> data;
			Log::Assert(size >= 0, "Cannot initalize a dynamic array of with a negative size.");
			if (size == 0)
			{
				// If a 0 size is passed in initialize an array sizeof 1
				size = 1;
			}

			data.m_Data = (T*)AllocMem(sizeof(T) * size);
			data.m_NumElements = 0;
			data.m_MaxSize = size;
			return data;
		}

		template<typename T>
		void Free(DynamicArray<T>& data)
		{
			if (data.m_Data && data.m_MaxSize > 0)
			{
				FreeMem(data.m_Data);
				data.m_Data = nullptr;
			}
		}

		template<typename T>
		void Add(DynamicArray<T>& data, const T& element)
		{
			NDynamicArrayPrivate::CheckResize<T>(data, 1);
			data.m_Data[data.m_NumElements] = element;
			data.m_NumElements++;
		}

		template<typename T>
		void Insert(DynamicArray<T>& data, const T& element, int index)
		{
			Log::Assert(index >= 0 && index < data.m_NumElements, "Index out of bounds exception. Cannot insert element at '%d' in array of size '%d'.", index, data.m_NumElements);
			NDynamicArrayPrivate::CheckResize<T>(data, 1);
			for (int i = data.m_NumElements + 1; i >= index; i--)
			{
				data.m_Data[i] = data.m_Data[i - 1];
			}
			data.m_Data[index] = element;
			data.m_NumElements++;
		}

		// Place will overwrite anything in the array at the range [index, index + sizeOfDataToAdd]
		// with whatever is stored in the T* pointer as long as index <= m_NumElements
		template<typename T>
		void Place(DynamicArray<T>& data, int index, const T* dataToAdd, uint32 numElementsToOverwrite)
		{
			Log::Assert(index >= 0 && index <= data.m_NumElements, "Index out of bounds exception. Cannot place data outside of array bounds, tried to place data at '%d' in array size '%d'", index, data.m_NumElements);
			NDynamicArrayPrivate::CheckResize<T>(data, (index + numElementsToOverwrite) - data.m_MaxSize);
			memcpy(&data.m_Data[index], dataToAdd, sizeof(T) * numElementsToOverwrite);
			if (index + numElementsToOverwrite > data.m_NumElements)
			{
				data.m_NumElements = index + numElementsToOverwrite;
			}
		}

		template<typename T>
		void Remove(DynamicArray<T>& data, int index)
		{
			for (int i = index; i < data.m_NumElements - 1; i++)
			{
				data.m_Data[i] = data.m_Data[i + 1];
			}

			data.m_NumElements--;
			if (data.m_NumElements < (data.m_MaxSize / 2))
			{
				data.m_MaxSize /= 2;
				data.m_Data = (T*)ReallocMem(data.m_Data, sizeof(T) * data.m_MaxSize);
			}
		}

		template<typename T>
		T& Get(DynamicArray<T>& data, int index)
		{
			Log::Assert(index >= 0 && index < data.m_NumElements, "Index out of bounds exception. '%d' in array size '%d'.", index, data.m_NumElements);
			return data.m_Data[index];
		}

		template<typename T>
		const T& Get(const DynamicArray<T>& data, int index)
		{
			Log::Assert(index >= 0 && index < data.m_NumElements, "Index out of bounds exception. '%d' in array size '%d'.", index, data.m_NumElements);
			return data.m_Data[index];
		}

		template<typename T>
		T* Begin(DynamicArray<T>& data)
		{
			return &data.m_Data[0];
		}

		template<typename T>
		T* End(DynamicArray<T>& data)
		{
			return &data.m_Data[0] + data.m_NumElements;
		}

		template<typename T>
		void Clear(DynamicArray<T>& data, bool freeMemory = true)
		{
			data.m_NumElements = 0;
			if (freeMemory)
			{
				if (data.m_MaxSize != 1)
				{
					data.m_MaxSize = 1;
					data.m_Data = (T*)ReallocMem(data.m_Data, sizeof(T) * data.m_MaxSize);
				}
			}
		}
	}
}