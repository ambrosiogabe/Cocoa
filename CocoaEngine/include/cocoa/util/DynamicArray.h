#pragma once
#include "externalLibs.h"
#include "cocoa/util/Log.h"

namespace Cocoa
{
	template<typename T>
	struct DynamicArrayData
	{
		T* m_Data;
		int m_NumElements;
		int m_MaxSize;
	};

	namespace DynamicArray
	{
		template<typename T>
		DynamicArrayData<T> Create(int size=0)
		{
			DynamicArrayData<T> data;
			Log::Assert(size >= 0, "Cannot initalize a dynamic array of with a negative size.");
			if (size == 0)
			{
				// If a 0 size is passed in initialize an array sizeof 1
				size = 1;
			}

			data.m_Data = (T*)malloc(sizeof(T) * size);
			data.m_NumElements = 0;
			data.m_MaxSize = size;
			return data;
		}

		template<typename T>
		void Free(DynamicArrayData<T>& data)
		{
			if (data.m_Data && data.m_MaxSize > 0)
			{
				free(data.m_Data);
				data.m_Data = nullptr;
			}
		}

		template<typename T>
		void Add(DynamicArrayData<T>& data, const T& element)
		{
			if (data.m_NumElements + 1 > data.m_MaxSize)
			{
				data.m_MaxSize *= 2;
				data.m_Data = (T*)realloc(data.m_Data, sizeof(T) * data.m_MaxSize);
			}

			data.m_Data[data.m_NumElements] = element;
			data.m_NumElements++;
		}

		template<typename T>
		void Remove(DynamicArrayData<T>& data, int index)
		{
			for (int i = index; i < data.m_NumElements - 1; i++)
			{
				data.m_Data[i] = data.m_Data[i + 1];
			}

			data.m_NumElements--;
			if (data.m_NumElements < (data.m_MaxSize / 2))
			{
				data.m_MaxSize /= 2;
				data.m_Data = (T*)realloc(data.m_Data, sizeof(T) * data.m_MaxSize);
			}
		}

		template<typename T>
		T& Get(DynamicArrayData<T>& data, int index)
		{
			Log::Assert(index >= 0 && index < data.m_NumElements, "Index out of bounds exception. '%d' in array size '%d'.", index, data.m_NumElements);
			return data.m_Data[index];
		}

		template<typename T>
		T* Begin(DynamicArrayData<T>& data)
		{
			return &data.m_Data[0];
		}

		template<typename T>
		T* End(DynamicArrayData<T>& data)
		{
			return &data.m_Data[0] + data.m_NumElements;
		}
	}
}