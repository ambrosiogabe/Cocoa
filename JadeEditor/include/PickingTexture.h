#pragma once
#include "externalLibs.h"

namespace Jade
{
	// This texture is an fbo that holds data that will be used for 
	// a 3D picking technique that uses pixel perfect picking by storing
	// entity id's at pixel values
	class PickingTexture
	{
	public:
		struct PixelInfo
		{
			float m_EntityID; // This is so that we maintain type consistency with opengl (will be cast to uint32 in the end)
			float m_DrawID;
			float m_PrimID;

			PixelInfo()
			{
				m_EntityID = 0.0f;
				m_DrawID = 0.0f;
				m_PrimID = 0.0f;
			}
		};

	public:
		PickingTexture();
		~PickingTexture();

		bool Init(uint32 windowWidth, uint32 windowHeight);

		void EnableWriting();
		void DisableWriting();

		inline uint32 GetPickingTextureID() const { return m_PickingTexture; }

		PixelInfo ReadPixel(uint32 x, uint32 y) const;

	private:
		// TODO: Figure out some way to make a factory for framebuffers, then just use the factory to create framebuffer instead of custom like this
		uint32 m_FBO;
		uint32 m_PickingTexture;
		uint32 m_DepthTexture;
	};
}