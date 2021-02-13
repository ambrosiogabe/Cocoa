#include "externalLibs.h"

#include "cocoa/renderer/Framebuffer.h"
#include "cocoa/renderer/Texture.h"
#include "cocoa/util/Log.h"
#include "cocoa/core/AssetManager.h"

namespace Cocoa
{
	namespace NFramebuffer
	{
		void Generate(Framebuffer& framebuffer)
		{
			Log::Assert(framebuffer.Fbo == (uint32)-1, "Cannot generate framebuffer that has with Fbo id == (uint32)-1.");
			Log::Assert(framebuffer.Rbo == (uint32)-1, "Cannot generate framebuffer that has with Rbo id == (uint32)-1.");

			glGenFramebuffers(1, &framebuffer.Fbo);
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.Fbo);

			// Create texture to render data to and attach it to framebuffer
			for (int i = 0; i < framebuffer.ColorAttachments.size(); i++)
			{
				Texture& texture = framebuffer.ColorAttachments[i];
				texture.Width = framebuffer.Width;
				texture.Height = framebuffer.Height;
				TextureUtil::Generate(texture);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texture.GraphicsId, 0);
			}

			if (framebuffer.IncludeDepthStencil)
			{
				Log::Assert(framebuffer.DepthStencilFormat != ByteFormat::None, "Cannot add depth stencil information with no byte format.");

				// Create renderbuffer to store depth_stencil info
				glGenRenderbuffers(1, &framebuffer.Rbo);
				glBindRenderbuffer(GL_RENDERBUFFER, framebuffer.Rbo);
				uint32 glDepthFormat = TextureUtil::ToGl(framebuffer.DepthStencilFormat);
				glRenderbufferStorage(GL_RENDERBUFFER, glDepthFormat, framebuffer.Width, framebuffer.Height);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, framebuffer.Rbo);
			}

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			{
				Log::Assert(false, "Framebuffer is not complete.");
			}

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void Delete(Framebuffer& framebuffer)
		{
			Log::Assert(framebuffer.Fbo != (uint32)-1, "Tried to delete invalid framebuffer.");
			glDeleteFramebuffers(1, &framebuffer.Fbo);
			framebuffer.Fbo = (uint32)-1;

			for (int i = 0; i < framebuffer.ColorAttachments.size(); i++)
			{
				Texture& texture = framebuffer.ColorAttachments[i];
				TextureUtil::Delete(texture);
			}
			framebuffer.ColorAttachments.clear();

			if (framebuffer.IncludeDepthStencil)
			{
				Log::Assert(framebuffer.Rbo != (uint32)-1, "Tried to delete invalid renderbuffer.");
				glDeleteRenderbuffers(1, &framebuffer.Rbo);
				framebuffer.Rbo = (uint32)-1;
			}
		}

		void AddColorAttachment(Framebuffer& framebuffer, const Texture& textureSpecification)
		{
			framebuffer.ColorAttachments.push_back(textureSpecification);
		}

		const Texture& GetColorAttachment(const Framebuffer& framebuffer, int index)
		{
			return framebuffer.ColorAttachments.at(index);
		}

		void Bind(const Framebuffer& framebuffer)
		{
			Log::Assert(framebuffer.Fbo != (uint32)-1, "Tried to bind invalid framebuffer.");
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.Fbo);
		}

		void Unbind(const Framebuffer& framebuffer)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	}
}