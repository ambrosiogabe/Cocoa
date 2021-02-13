#pragma once
#include "cocoa/core/Core.h"
#include "externalLibs.h"

// TODO: Refactor this into a framebuffer!!! 
namespace Cocoa
{
	// This texture is an fbo that holds data that will be used for 
	// a 3D picking technique that uses pixel perfect picking by storing
	// entity id's at pixel values
	class COCOA PickingTexture
	{
	public:
		struct PixelInfo
		{
			uint32 m_EntityID;

			PixelInfo()
			{
				m_EntityID = 0;
			}

			PixelInfo(uint32 id)
			{
				m_EntityID = id;
			}
		};

	public:
		PickingTexture() 
		{
			m_FBO = -1;
			m_PickingTexture = -1;
			m_DepthTexture = -1;
		}

		PickingTexture(uint32 windowWidth, uint32 windowHeight);
		~PickingTexture();

		void Bind() const { glBindFramebuffer(GL_FRAMEBUFFER, m_FBO); }
		void Unbind() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

		inline uint32 GetPickingTextureID() const { return m_PickingTexture; }

		PixelInfo ReadPixel(uint32 x, uint32 y) const;

	private:
		bool Init(uint32 windowWidth, uint32 windowHeight);

	private:
		// TODO: Figure out some way to make a factory for framebuffers, then just use the factory to create framebuffer instead of custom like this
		uint32 m_FBO = -1;
		uint32 m_PickingTexture;
		uint32 m_DepthTexture;
	};
}