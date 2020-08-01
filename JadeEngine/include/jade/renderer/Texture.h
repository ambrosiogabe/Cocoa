#pragma once

#include "jade/physics2d/Physics2D.h"
#include "jade/core/Core.h"
#include "jade/core/AssetManager.h"

namespace Jade
{
	class Texture : public Asset
	{
	public:
		Texture(const JPath& resourceName);
		Texture(int width, int height);

		virtual void Load() override;
		virtual void Unload() override;

		void Bind();
		void Unbind();

		int GetId() const { return m_ID; }
		int GetWidth() const { return m_Width; }
		int GetHeight() const { return m_Height; }
		const JPath& GetFilepath() const { return m_Path; }
		void FreePixels();
		const uint8* GetPixelBuffer();
		int BytesPerPixel() const { return m_BytesPerPixel; }

	private:
		unsigned int m_ID;
		int m_Width, m_Height;
		int m_BytesPerPixel = 3;

		uint8* m_PixelBuffer = nullptr;
		bool m_PixelsFreed = false;

		AABB m_BoundingBox;
	};
}