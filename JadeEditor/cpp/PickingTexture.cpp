#include "PickingTexture.h"
#include "jade/util/Log.h"

namespace Jade
{
	PickingTexture::PickingTexture()
	{

	}

	PickingTexture::~PickingTexture()
	{
		glDeleteTextures(1, &m_PickingTexture);
		glDeleteTextures(1, &m_DepthTexture);
		glDeleteFramebuffers(1, &m_FBO);
	}

	bool PickingTexture::Init(uint32 windowWidth, uint32 windowHeight)
	{
		// Create the FBO
		glGenFramebuffers(1, &m_FBO);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

		// Create the texture object for the primitive information
		glGenTextures(1, &m_PickingTexture);
		glBindTexture(GL_TEXTURE_2D, m_PickingTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, windowWidth, windowHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_PickingTexture, 0);

		// Create the texture object for the depth buffer
		glEnable(GL_TEXTURE_2D);
		glGenTextures(1, &m_DepthTexture);
		glBindTexture(GL_TEXTURE_2D, m_DepthTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, windowWidth, windowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthTexture, 0);

		// Disable reading to avoid problems with older GPU's
		glReadBuffer(GL_NONE);

		glDrawBuffer(GL_COLOR_ATTACHMENT0);

		// Verify that the FBO is correct
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			const char* errorCode = nullptr;
			if (status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)
				errorCode = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
			else if (status == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)
				errorCode = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
			else if (status == GL_FRAMEBUFFER_UNSUPPORTED)
				errorCode = "GL_FRAMEBUFFER_UNSUPPORTED";
			else
				errorCode = "UNKNOWN";
			Log::Error("Framebuffer error (GL ERROR): 0x%x::%s", status, errorCode);
			return false;
		}

		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			Log::Error("Error creating framebuffer (GL ERROR): 0x%x", error);
			return false;
		}

		// Restore the default framebuffer 
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return true;
	}

	void PickingTexture::EnableWriting()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FBO);
	}

	void PickingTexture::DisableWriting()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}

	PickingTexture::PixelInfo PickingTexture::ReadPixel(uint32 x, uint32 y) const
	{
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FBO);
		glReadBuffer(GL_COLOR_ATTACHMENT0);

		PixelInfo pixel;
		glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, &pixel);
		if (pixel.m_EntityID < 1)
		{
			pixel.m_EntityID = 0xffffffff;
			pixel.m_DrawID = 0xffffffff;
		}
		else
		{
			pixel.m_EntityID--;
		}

		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

		return pixel;
	}
}