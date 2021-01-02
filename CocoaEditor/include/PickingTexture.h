#pragma once
#include "externalLibs.h"

namespace Cocoa
{
	// This texture is an fbo that holds data that will be used for 
	// a 3D picking technique that uses pixel perfect picking by storing
	// entity id's at pixel values
	class PickingTexture
	{
	public:
		struct PixelInfo
		{
			uint32 m_EntityID;

			PixelInfo()
			{
				m_EntityID = 0;
			}
		};

	public:
		PickingTexture(uint32 windowWidth, uint32 windowHeight);
		~PickingTexture();

		void EnableWriting();
		void DisableWriting();

		inline uint32 GetPickingTextureID() const { return m_PickingTexture; }

		PixelInfo ReadPixel(uint32 x, uint32 y) const;

	private:
		bool Init(uint32 windowWidth, uint32 windowHeight);

	private:
		// TODO: Figure out some way to make a factory for framebuffers, then just use the factory to create framebuffer instead of custom like this
		uint32 m_FBO;
		uint32 m_PickingTexture;
		uint32 m_DepthTexture;
	};
}