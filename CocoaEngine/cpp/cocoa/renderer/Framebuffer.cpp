#include "externalLibs.h"

#include "cocoa/renderer/Framebuffer.h"
#include "cocoa/renderer/Texture.h"
#include "cocoa/util/Log.h"
#include "cocoa/util/JsonExtended.h"
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

			if (framebuffer.ColorAttachments.size() > 1)
			{
				Log::Assert(framebuffer.ColorAttachments.size() < 8, "Too many framebuffer attachments. Only 8 attachments supported.");
				static GLenum colorBufferAttachments[8] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7 };
				glDrawBuffers(framebuffer.ColorAttachments.size(), colorBufferAttachments);
			}

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

		void Regenerate(Framebuffer& framebuffer)
		{
			Log::Assert(framebuffer.Fbo != (uint32)-1, "Cannot regenerate framebuffer that has with Fbo id != (uint32)-1.");
			if (framebuffer.IncludeDepthStencil)
			{
				Log::Assert(framebuffer.Rbo != (uint32)-1, "Cannot regenerate framebuffer that has with Rbo id != (uint32)-1.");
			}

			Delete(framebuffer, false);
			Generate(framebuffer);
		}

		void Delete(Framebuffer& framebuffer, bool clearColorAttachmentSpecs)
		{
			Log::Assert(framebuffer.Fbo != (uint32)-1, "Tried to delete invalid framebuffer.");
			glDeleteFramebuffers(1, &framebuffer.Fbo);
			framebuffer.Fbo = (uint32)-1;

			for (int i = 0; i < framebuffer.ColorAttachments.size(); i++)
			{
				Texture& texture = framebuffer.ColorAttachments[i];
				TextureUtil::Delete(texture);
			}

			if (clearColorAttachmentSpecs)
			{
				framebuffer.ColorAttachments.clear();
			}

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

		void ClearColorAttachmentRgb(const Framebuffer& framebuffer, int colorAttachment, glm::vec3 clearColor)
		{
			Log::Assert(colorAttachment >= 0 && colorAttachment < framebuffer.ColorAttachments.size(), "Index out of bounds. Color attachment does not exist '%d'.", colorAttachment);
			const Texture& texture = framebuffer.ColorAttachments[colorAttachment];
			Log::Assert(TextureUtil::ByteFormatIsRgb(texture.InternalFormat) && TextureUtil::ByteFormatIsRgb(texture.ExternalFormat), "Cannot clear non-rgb texture as if it were a rgb texture.");

			uint32 externalFormat = TextureUtil::ToGl(texture.ExternalFormat);
			uint32 formatType = TextureUtil::ToGlDataType(texture.ExternalFormat);
			glClearTexImage(texture.GraphicsId, 0, externalFormat, formatType, &clearColor);
		}

		void ClearColorAttachmentUint32(const Framebuffer& framebuffer, int colorAttachment, uint32 clearColor)
		{
			Log::Assert(colorAttachment >= 0 && colorAttachment < framebuffer.ColorAttachments.size(), "Index out of bounds. Color attachment does not exist '%d'.", colorAttachment);
			const Texture& texture = framebuffer.ColorAttachments[colorAttachment];
			Log::Assert(TextureUtil::ByteFormatIsInt(texture.InternalFormat) && TextureUtil::ByteFormatIsInt(texture.ExternalFormat), "Cannot clear non-uint texture as if it were a uint texture.");

			uint32 externalFormat = TextureUtil::ToGl(texture.ExternalFormat);
			uint32 formatType = TextureUtil::ToGlDataType(texture.ExternalFormat);
			glClearTexImage(texture.GraphicsId, 0, externalFormat, formatType, &clearColor);
		}

		uint32 ReadPixelUint32(const Framebuffer& framebuffer, int colorAttachment, int x, int y)
		{
			Log::Assert(colorAttachment >= 0 && colorAttachment < framebuffer.ColorAttachments.size(), "Index out of bounds. Color attachment does not exist '%d'.", colorAttachment);
			const Texture& texture = framebuffer.ColorAttachments[colorAttachment];
			Log::Assert(TextureUtil::ByteFormatIsInt(texture.InternalFormat) && TextureUtil::ByteFormatIsInt(texture.ExternalFormat), "Cannot read non-uint texture as if it were a uint texture.");

			// If we are requesting an out of bounds pixel, return max uint32 which should be a good flag I guess
			// TODO: Create clearColor member variable in color attachments and return that instead here
			if (x < 0 || y < 0 || x >= framebuffer.ColorAttachments[colorAttachment].Width || y >= framebuffer.ColorAttachments[colorAttachment].Height)
			{
				return (uint32)-1;
			}

			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.Fbo);
			glReadBuffer(GL_COLOR_ATTACHMENT0 + colorAttachment);

			// 128 bits should be big enough for 1 pixel of any format
			// TODO: Come up with generic way to get any type of pixel data
			static uint32 pixel;
			uint32 externalFormat = TextureUtil::ToGl(texture.ExternalFormat);
			uint32 formatType = TextureUtil::ToGlDataType(texture.ExternalFormat);
			glReadPixels(x, y, 1, 1, externalFormat, formatType, &pixel);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			return pixel;
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

		json Serialize(const Framebuffer& framebuffer)
		{
			json res;
			res["Width"] = framebuffer.Width;
			res["Height"] = framebuffer.Height;
			res["IncludeDepthStencil"] = framebuffer.IncludeDepthStencil;
			res["DepthStencilFormat"] = framebuffer.DepthStencilFormat;
			res["ColorAttachments"] = {};

			for (int i = 0; i < framebuffer.ColorAttachments.size(); i++)
			{
				res["ColorAttachments"][i] = TextureUtil::Serialize(framebuffer.ColorAttachments[i]);
			}

			return res;
		}

		Framebuffer Deserialize(const json& j)
		{
			Framebuffer res;
			JsonExtended::AssignIfNotNull(j, "Width", res.Width);
			JsonExtended::AssignIfNotNull(j, "Height", res.Height);
			JsonExtended::AssignIfNotNull(j, "IncludeDepthStencil", res.IncludeDepthStencil);

			if (j["ColorAttachments"].size() > 0)
			{
				const int colorAttachmentSize = j["ColorAttachments"].size();
				for (int i = 0; i < colorAttachmentSize; i++)
				{
					if (j["ColorAttachments"][i].is_object())
					{
						AddColorAttachment(res, TextureUtil::Deserialize(j["ColorAttachments"][i]));
					}
					else
					{
						Log::Warning("Invalid color attachment '%d' when deserializing framebuffer.", i);
					}
				}
			}

			return res;
		}
	}
}