#include "externalLibs.h"

#include "cocoa/renderer/Framebuffer.h"
#include "cocoa/renderer/Texture.h"
#include "cocoa/util/Log.h"
#include "cocoa/core/AssetManager.h"

namespace Cocoa
{
	void Framebuffer::Init()
	{
		glGenFramebuffers(1, &m_ID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_ID);

		// Create texture to render data to and attach it to framebuffer
		m_Texture = {};
		m_Texture.Width = m_Width;
		m_Texture.Height = m_Height;
		m_Texture.MinFilter = FilterMode::Linear;
		m_Texture.MagFilter = FilterMode::Linear;
		m_Texture.InternalFormat = ByteFormat::RGB;
		TextureUtil::Generate(m_Texture);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Texture.GraphicsId, 0);

		// Create renderbuffer to store depth_stencil info
		glGenRenderbuffers(1, &m_RboID);
		glBindRenderbuffer(GL_RENDERBUFFER, m_RboID);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RboID);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			Log::Assert(false, "Framebuffer is not complete.");
		}

		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			Log::Error("Error creating framebuffer (GL ERROR): 0x%x", error);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Framebuffer::Delete()
	{
		glDeleteFramebuffers(1, &m_ID);
		glDeleteRenderbuffers(1, &m_RboID);
		glDeleteTextures(1, &m_Texture.GraphicsId);
	}

	Framebuffer& Framebuffer::AddAttachment(TextureSpecification textureSpecification)
	{
		//// Create texture to render data to and attach it to framebuffer
		//Handle<Texture> texture = AssetManager::CreateInternalTexture(m_Width, m_Height);
		//m_Texture = new Texture(m_Width, m_Height);
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Texture->GetId(), 0);

		//// Create renderbuffer to store depth_stencil info
		//unsigned int rboID;
		//glGenRenderbuffers(1, &rboID);
		//glBindRenderbuffer(GL_RENDERBUFFER, rboID);
		//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height);
		//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboID);
		return *this;
	}
}