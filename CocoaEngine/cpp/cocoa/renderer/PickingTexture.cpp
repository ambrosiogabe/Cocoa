#include "cocoa/renderer/PickingTexture.h"
#include "cocoa/util/Log.h"

namespace Cocoa
{
	PickingTexture::PickingTexture(uint32 windowWidth, uint32 windowHeight)
	{
		if (!Init(windowWidth, windowHeight))
		{
			Log::Error("Error initializing picking texture.");
			Log::Assert(false, "");
		}
	}

	PickingTexture::~PickingTexture()
	{
		if (m_PickingTexture != -1)
		{
			glDeleteTextures(1, &m_PickingTexture);
			glDeleteTextures(1, &m_DepthTexture);
			glDeleteFramebuffers(1, &m_FBO);
		}
	}

	bool PickingTexture::Init(uint32 windowWidth, uint32 windowHeight)
	{
		// Create the FBO
		m_FBO = 0;
		glGenFramebuffers(1, &m_FBO);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

		// Create the texture object for the primitive information
		glGenTextures(1, &m_PickingTexture);
		glBindTexture(GL_TEXTURE_2D, m_PickingTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, windowWidth, windowHeight, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_PickingTexture, 0);

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

		// Restore the default framebuffer 
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		return true;
	}

	PickingTexture::PixelInfo PickingTexture::ReadPixel(uint32 x, uint32 y) const
	{
		glReadBuffer(GL_COLOR_ATTACHMENT0);

		uint32 pixel;
		glReadPixels(1, 1, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, &pixel);
		if (pixel < 1)
		{
			pixel = std::numeric_limits<uint32>::max();
		}
		else
		{
			pixel--;
		}

		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

		return PixelInfo(pixel);
	}
}