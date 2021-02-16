#include "externalLibs.h"
#include "cocoa/core/Memory.h"
#include "cocoa/util/Log.h"

namespace Cocoa
{
	namespace Memory
	{
#if _COCOA_DEBUG
		struct DebugMemoryAllocation
		{
			const char* FileAllocator;
			int FileAllocatorLine;
			int References;
			void* Memory;

			bool operator==(const DebugMemoryAllocation& other) const
			{
				return other.Memory == this->Memory;
			}
		};

		static std::vector<DebugMemoryAllocation> InternalAllocations;
#endif

		void* _Allocate(const char* filename, int line, size_t numBytes)
		{
			void* memory = malloc(numBytes);
#if _COCOA_DEBUG
			// If we are in a debug build, track all memory allocations to see if we free them all as well
			auto iterator = std::find(InternalAllocations.begin(), InternalAllocations.end(), DebugMemoryAllocation{ filename, line, 0, memory });
			if (iterator == InternalAllocations.end())
			{
				InternalAllocations.push_back({ filename, line, 1, memory });
			}
			else
			{
				if (iterator->References <= 0)
				{
					iterator->References++;
				}
				else
				{
					Log::Error("Tried to allocate memory that has already been allocated... This should never be hit. If it is, we have a problem.");
				}
			}
#endif
			return memory;
		}

		void* _Realloc(const char* filename, int line, void* oldMemory, size_t numBytes)
		{
			void* newMemory = realloc(oldMemory, numBytes);
#if _COCOA_DEBUG
			// If we are in a debug build, track all memory allocations to see if we free them all as well
			auto newMemoryIter = std::find(InternalAllocations.begin(), InternalAllocations.end(), DebugMemoryAllocation{ filename, line, 0, newMemory });
			auto oldMemoryIter = std::find(InternalAllocations.begin(), InternalAllocations.end(), DebugMemoryAllocation{ filename, line, 0, oldMemory });
			if (newMemoryIter != oldMemoryIter)
			{
				// Realloc could not expand the current pointer, so it allocated a new memory block
				if (oldMemoryIter == InternalAllocations.end())
				{
					Log::Error("Tried to realloc invalid memory in '%s' line: %d.", filename, line);
				}
				else
				{
					oldMemoryIter->References--;
				}

				if (newMemoryIter == InternalAllocations.end())
				{
					InternalAllocations.push_back({ filename, line, 1, newMemory });
				}
				else
				{
					if (newMemoryIter->References <= 0)
					{
						newMemoryIter->References++;
					}
					else
					{
						Log::Error("Tried to allocate memory that has already been allocated... This should never be hit. If it is, we have a problem.");
					}
				}
			}
			// If realloc expanded the memory in-place, then we don't need to do anything because no "new" memory locations were allocated
#endif
			return newMemory;
		}

		void _Free(const char* filename, int line, void* memory)
		{
#if _COCOA_DEBUG
			auto iterator = std::find(InternalAllocations.begin(), InternalAllocations.end(), DebugMemoryAllocation{ filename, line, 0, memory });
			if (iterator == InternalAllocations.end())
			{
				Log::Error("Tried to free invalid memory that was never allocated.");
				Log::Error("Code that attempted to free: '%s' line: %d", filename, line);
			}
			else if (iterator->References <= 0)
			{
				Log::Error("Tried to free memory that has already been freed.");
				Log::Error("Code that attempted to free: '%s' line: %d", filename, line);
				Log::Error("Code that allocated the memory block: '%s' line: %d", iterator->FileAllocator, iterator->FileAllocatorLine);
			}
			else
			{
				iterator->References--;
			}
#endif
			// When debug is turned off we literally just free the memory, so it will through a segfault if a
			// faulty release build was published
			free(memory);
		}

		void Init()
		{
#if _COCOA_DEBUG
			InternalAllocations = std::vector<DebugMemoryAllocation>();
#endif
		}

		void Destroy()
		{
#if _COCOA_DEBUG
			for (auto& alloc : InternalAllocations)
			{
				if (alloc.References > 0)
				{
					Log::Warning("Application ended execution and did not free memory allocated at: '%s' line: %d", alloc.FileAllocator, alloc.FileAllocatorLine);
				}
			}
#endif
		}
	}
}