#pragma once

#include "cocoa/physics2d/Physics2D.h"
#include "cocoa/core/Core.h"

namespace Cocoa
{
	class COCOA Texture
	{
	public:
		Texture();
		Texture(const CPath& resourceName, bool isDefault=false);
		Texture(int width, int height, bool isDefault=false);

		void Load();
		void Delete();
		json Serialize();

		void Bind() const;
		void Unbind() const;

		void FreePixels();
		const uint8* GetPixelBuffer() const;

		inline int GetId() const { return m_ID; }
		inline int GetWidth() const { return m_Width; }
		inline int GetHeight() const { return m_Height; }
		inline const CPath& GetFilepath() const { return m_Path; }
		inline int BytesPerPixel() const { return m_BytesPerPixel; }
		inline bool IsDefault() const { return m_IsDefault; }
		inline bool IsNull() { return m_IsNull; }

	public:
		static Texture nullTexture;

	private:
		bool m_IsDefault;
		bool m_IsNull = false;
		CPath m_Path;
		unsigned int m_ID;
		int m_Width, m_Height;
		int m_BytesPerPixel = 3;

		uint8* m_PixelBuffer = nullptr;
		bool m_PixelsFreed = false;

		AABB m_BoundingBox;
	};
}