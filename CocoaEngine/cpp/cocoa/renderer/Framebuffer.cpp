#include "externalLibs.h"

#include "cocoa/renderer/Framebuffer.h"
#include "cocoa/renderer/Texture.h"
#include "cocoa/util/Log.h"

namespace Cocoa
{
	void Framebuffer::Init()
	{
		glGenFramebuffers(1, &m_ID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_ID);

		// Create texture to render data too and attach it to framebuffer
		m_Texture = new Texture(m_Width, m_Height);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Texture->GetId(), 0);

		// Create renderbuffer to store depth_stencil info
		unsigned int rboID;
		glGenRenderbuffers(1, &rboID);
		glBindRenderbuffer(GL_RENDERBUFFER, rboID);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboID);

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
}